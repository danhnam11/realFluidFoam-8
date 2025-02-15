/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2011-2020 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "PRchungTakaStandardChemistryModel.H"
#include "PRchungTakaMixture.H"
#include "UniformField.H"
#include "extrapolatedCalculatedFvPatchFields.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class ReactionThermo, class ThermoType>
Foam::PRchungTakaStandardChemistryModel<ReactionThermo, ThermoType>::PRchungTakaStandardChemistryModel
(
    const ReactionThermo& thermo
)
:
    BasicChemistryModel<ReactionThermo>(thermo),
    ODESystem(),
    Y_(this->thermo().composition().Y()),
    specieThermos_
    (
        dynamic_cast<const PRchungTakaMixture<ThermoType>&>
            (this->thermo()).specieThermos()
    ),
    reactions_
    (
        dynamic_cast<const PRchungTakaMixture<ThermoType>&>
        (
            this->thermo()
        ).species(),
        specieThermos_,
        this->mesh(),
        *this
    ),

    nSpecie_(Y_.size()),
    nReaction_(reactions_.size()),
    Treact_
    (
        BasicChemistryModel<ReactionThermo>::template lookupOrDefault<scalar>
        (
            "Treact",
            0
        )
    ),
    RR_(nSpecie_),
    c_(nSpecie_),
    dcdt_(nSpecie_)
{
    // Create the fields for the chemistry sources
    forAll(RR_, fieldi)
    {
        RR_.set
        (
            fieldi,
            new volScalarField::Internal
            (
                IOobject
                (
                    "RR." + Y_[fieldi].name(),
                    this->mesh().time().timeName(),
                    this->mesh(),
                    IOobject::NO_READ,
                    IOobject::NO_WRITE
                ),
                thermo.T().mesh(),
                dimensionedScalar(dimMass/dimVolume/dimTime, 0)
            )
        );
    }

    Info<< "PRchungTakaStandardChemistryModel: Number of species = " << nSpecie_
        << " and reactions = " << nReaction_ << endl;
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class ReactionThermo, class ThermoType>
Foam::PRchungTakaStandardChemistryModel<ReactionThermo, ThermoType>::
~PRchungTakaStandardChemistryModel()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class ReactionThermo, class ThermoType>
void Foam::PRchungTakaStandardChemistryModel<ReactionThermo, ThermoType>::omega
(
    const scalar p,
    const scalar T,
    const scalarField& c,
    const label li,
    scalarField& dcdt
) const
{

    dcdt = Zero;

    forAll(reactions_, i)
    {
        const Reaction<ThermoType>& R = reactions_[i];

        R.omega(p, T, c, li, dcdt);
    }
}


template<class ReactionThermo, class ThermoType>
Foam::scalar Foam::PRchungTakaStandardChemistryModel<ReactionThermo, ThermoType>::omegaI
(
    const label index,
    const scalar p,
    const scalar T,
    const scalarField& c,
    const label li,
    scalar& pf,
    scalar& cf,
    label& lRef,
    scalar& pr,
    scalar& cr,
    label& rRef
) const
{
    const Reaction<ThermoType>& R = reactions_[index];
    scalar w = R.omega(p, T, c, li, pf, cf, lRef, pr, cr, rRef);
    return(w);
}


template<class ReactionThermo, class ThermoType>
void Foam::PRchungTakaStandardChemistryModel<ReactionThermo, ThermoType>::derivatives
(
    const scalar time,
    const scalarField& c,
    const label li,
    scalarField& dcdt
) const
{
    const scalar T = c[nSpecie_];
    const scalar p = c[nSpecie_ + 1];

    forAll(c_, i)
    {
        c_[i] = max(c[i], 0);
    }

    omega(p, T, c_, li, dcdt);

    // Constant pressure
    // dT/dt = ...
    scalar rho = 0;
    scalar cSum = 0;
    for (label i = 0; i < nSpecie_; i++)
    {
        const scalar W = specieThermos_[i].W();
        cSum += c_[i];
        rho += W*c_[i];
    }
    scalar cp = 0;
    for (label i=0; i<nSpecie_; i++)
    {
        cp += c_[i]*specieThermos_[i].cp(p, T);
    }
    cp /= rho;

    scalar dT = 0;
    for (label i = 0; i < nSpecie_; i++)
    {
        const scalar hi = specieThermos_[i].ha(p, T);
        dT += hi*dcdt[i];
    }
    dT /= rho*cp;

    dcdt[nSpecie_] = -dT;

    // dp/dt = ...
    dcdt[nSpecie_ + 1] = 0;
}


template<class ReactionThermo, class ThermoType>
void Foam::PRchungTakaStandardChemistryModel<ReactionThermo, ThermoType>::jacobian
(
    const scalar t,
    const scalarField& c,
    const label li,
    scalarField& dcdt,
    scalarSquareMatrix& J
) const
{
    const scalar T = c[nSpecie_];
    const scalar p = c[nSpecie_ + 1];

    forAll(c_, i)
    {
        c_[i] = max(c[i], 0);
    }

    J = Zero;
    dcdt = Zero;

    // To compute the species derivatives of the temperature term,
    // the enthalpies of the individual species is needed
    scalarField hi(nSpecie_);
    scalarField cpi(nSpecie_);
    for (label i = 0; i < nSpecie_; i++)
    {
        hi[i] = specieThermos_[i].ha(p, T);
        cpi[i] = specieThermos_[i].cp(p, T);
    }
    scalar omegaI = 0;
    List<label> dummy;
    forAll(reactions_, ri)
    {
        const Reaction<ThermoType>& R = reactions_[ri];
        scalar kfwd, kbwd;
        R.dwdc(p, T, c_, li, J, dcdt, omegaI, kfwd, kbwd, false, dummy);
        R.dwdT(p, T, c_, li, omegaI, kfwd, kbwd, J, false, dummy, nSpecie_);
    }

    // The species derivatives of the temperature term are partially computed
    // while computing dwdc, they are completed hereunder:
    scalar cpMean = 0;
    scalar dcpdTMean = 0;
    for (label i=0; i<nSpecie_; i++)
    {
        cpMean += c_[i]*cpi[i]; // J/(m^3 K)
        dcpdTMean += c_[i]*specieThermos_[i].dcpdT(p, T);
    }
    scalar dTdt = 0.0;
    for (label i=0; i<nSpecie_; i++)
    {
        dTdt += hi[i]*dcdt[i]; // J/(m^3 s)
    }
    dTdt /= -cpMean; // K/s

    for (label i = 0; i < nSpecie_; i++)
    {
        J(nSpecie_, i) = 0;
        for (label j = 0; j < nSpecie_; j++)
        {
            J(nSpecie_, i) += hi[j]*J(j, i);
        }
        J(nSpecie_, i) += cpi[i]*dTdt; // J/(mol s)
        J(nSpecie_, i) /= -cpMean;    // K/s/(mol/m3)
    }

    // ddT of dTdt
    J(nSpecie_, nSpecie_) = 0;
    for (label i = 0; i < nSpecie_; i++)
    {
        J(nSpecie_, nSpecie_) += cpi[i]*dcdt[i] + hi[i]*J(i, nSpecie_);
    }
    J(nSpecie_, nSpecie_) += dTdt*dcpdTMean;
    J(nSpecie_, nSpecie_) /= -cpMean;
    J(nSpecie_, nSpecie_) += dTdt/T;
}


template<class ReactionThermo, class ThermoType>
Foam::tmp<Foam::volScalarField>
Foam::PRchungTakaStandardChemistryModel<ReactionThermo, ThermoType>::tc() const
{
    tmp<volScalarField> ttc
    (
        volScalarField::New
        (
            "tc",
            this->mesh(),
            dimensionedScalar(dimTime, small),
            extrapolatedCalculatedFvPatchScalarField::typeName
        )
    );

    scalarField& tc = ttc.ref();

    tmp<volScalarField> trho(this->thermo().rho());
    const scalarField& rho = trho();

    const scalarField& T = this->thermo().T();
    const scalarField& p = this->thermo().p();

    const label nReaction = reactions_.size();

    scalar pf, cf, pr, cr;
    label lRef, rRef;

    if (this->chemistry_)
    {
        forAll(rho, celli)
        {
            const scalar rhoi = rho[celli];
            const scalar Ti = T[celli];
            const scalar pi = p[celli];

            scalar cSum = 0;

            for (label i=0; i<nSpecie_; i++)
            {
                c_[i] = rhoi*Y_[i][celli]/specieThermos_[i].W();
                cSum += c_[i];
            }

            forAll(reactions_, i)
            {
                const Reaction<ThermoType>& R = reactions_[i];

                R.omega(pi, Ti, c_, celli, pf, cf, lRef, pr, cr, rRef);

                forAll(R.rhs(), s)
                {
                    tc[celli] += R.rhs()[s].stoichCoeff*pf*cf;
                }
            }

            tc[celli] = nReaction*cSum/tc[celli];
        }
    }

    ttc.ref().correctBoundaryConditions();

    return ttc;
}


template<class ReactionThermo, class ThermoType>
Foam::tmp<Foam::volScalarField>
Foam::PRchungTakaStandardChemistryModel<ReactionThermo, ThermoType>::Qdot() const
{
    tmp<volScalarField> tQdot
    (
        volScalarField::New
        (
            "Qdot",
            this->mesh_,
            dimensionedScalar(dimEnergy/dimVolume/dimTime, 0)
        )
    );

    if (this->chemistry_)
    {
        scalarField& Qdot = tQdot.ref();

        forAll(Y_, i)
        {
            forAll(Qdot, celli)
            {
                const scalar hi = specieThermos_[i].Hf();
                Qdot[celli] -= hi*RR_[i][celli];
            }
        }
    }

    return tQdot;
}


template<class ReactionThermo, class ThermoType>
Foam::tmp<Foam::DimensionedField<Foam::scalar, Foam::volMesh>>
Foam::PRchungTakaStandardChemistryModel<ReactionThermo, ThermoType>::calculateRR
(
    const label ri,
    const label si
) const
{
    tmp<volScalarField::Internal> tRR
    (
        volScalarField::Internal::New
        (
            "RR",
            this->mesh(),
            dimensionedScalar(dimMass/dimVolume/dimTime, 0)
        )
    );

    volScalarField::Internal& RR = tRR.ref();

    tmp<volScalarField> trho(this->thermo().rho());
    const scalarField& rho = trho();

    const scalarField& T = this->thermo().T();
    const scalarField& p = this->thermo().p();

    scalar pf, cf, pr, cr;
    label lRef, rRef;

    forAll(rho, celli)
    {
        const scalar rhoi = rho[celli];
        const scalar Ti = T[celli];
        const scalar pi = p[celli];

        for (label i=0; i<nSpecie_; i++)
        {
            const scalar Yi = Y_[i][celli];
            c_[i] = rhoi*Yi/specieThermos_[i].W();
        }

        const Reaction<ThermoType>& R = reactions_[ri];
        const scalar omegai = R.omega
        (
            pi, Ti, c_, celli, pf, cf, lRef, pr, cr, rRef
        );

        forAll(R.lhs(), s)
        {
            if (si == R.lhs()[s].index)
            {
                RR[celli] -= R.lhs()[s].stoichCoeff*omegai;
            }
        }

        forAll(R.rhs(), s)
        {
            if (si == R.rhs()[s].index)
            {
                RR[celli] += R.rhs()[s].stoichCoeff*omegai;
            }
        }

        RR[celli] *= specieThermos_[si].W();
    }

    return tRR;
}


template<class ReactionThermo, class ThermoType>
void Foam::PRchungTakaStandardChemistryModel<ReactionThermo, ThermoType>::calculate()
{
    if (!this->chemistry_)
    {
        return;
    }

    tmp<volScalarField> trho(this->thermo().rho());
    const scalarField& rho = trho();

    const scalarField& T = this->thermo().T();
    const scalarField& p = this->thermo().p();

    forAll(rho, celli)
    {
        const scalar rhoi = rho[celli];
        const scalar Ti = T[celli];
        const scalar pi = p[celli];

        for (label i=0; i<nSpecie_; i++)
        {
            const scalar Yi = Y_[i][celli];
            c_[i] = rhoi*Yi/specieThermos_[i].W();
        }

        omega(pi, Ti, c_, celli, dcdt_);

        for (label i=0; i<nSpecie_; i++)
        {
            RR_[i][celli] = dcdt_[i]*specieThermos_[i].W();
        }
    }
}


template<class ReactionThermo, class ThermoType>
template<class DeltaTType>
Foam::scalar Foam::PRchungTakaStandardChemistryModel<ReactionThermo, ThermoType>::solve
(
    const DeltaTType& deltaT
)
{
    BasicChemistryModel<ReactionThermo>::correct();

    scalar deltaTMin = great;

    if (!this->chemistry_)
    {
        return deltaTMin;
    }

    tmp<volScalarField> trho(this->thermo().rho());
    const scalarField& rho = trho();

    const scalarField& T = this->thermo().T();
    const scalarField& p = this->thermo().p();

    scalarField c0(nSpecie_);

    forAll(rho, celli)
    {
        scalar Ti = T[celli];

        if (Ti > Treact_)
        {
            const scalar rhoi = rho[celli];
            scalar pi = p[celli];

            for (label i=0; i<nSpecie_; i++)
            {
                c_[i] = rhoi*Y_[i][celli]/specieThermos_[i].W();
                c0[i] = c_[i];
            }

            // Initialise time progress
            scalar timeLeft = deltaT[celli];

            // Calculate the chemical source terms
            while (timeLeft > small)
            {
                scalar dt = timeLeft;
                this->solve(pi, Ti, c_, celli, dt, this->deltaTChem_[celli]);
                timeLeft -= dt;
            }

            deltaTMin = min(this->deltaTChem_[celli], deltaTMin);

            this->deltaTChem_[celli] =
                min(this->deltaTChem_[celli], this->deltaTChemMax_);

            for (label i=0; i<nSpecie_; i++)
            {
                RR_[i][celli] =
                    (c_[i] - c0[i])*specieThermos_[i].W()/deltaT[celli];
            }
        }
        else
        {
            for (label i=0; i<nSpecie_; i++)
            {
                RR_[i][celli] = 0;
            }
        }
    }

    return deltaTMin;
}


template<class ReactionThermo, class ThermoType>
Foam::scalar Foam::PRchungTakaStandardChemistryModel<ReactionThermo, ThermoType>::solve
(
    const scalar deltaT
)
{
    // Don't allow the time-step to change more than a factor of 2
    return min
    (
        this->solve<UniformField<scalar>>(UniformField<scalar>(deltaT)),
        2*deltaT
    );
}


template<class ReactionThermo, class ThermoType>
Foam::scalar Foam::PRchungTakaStandardChemistryModel<ReactionThermo, ThermoType>::solve
(
    const scalarField& deltaT
)
{
    return this->solve<scalarField>(deltaT);
}


// ************************************************************************* //
