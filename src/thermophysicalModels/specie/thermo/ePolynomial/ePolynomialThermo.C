/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2020 OpenFOAM Foundation
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

#include "ePolynomialThermo.H"
#include "IOstreams.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class EquationOfState, int PolySize>
Foam::ePolynomialThermo<EquationOfState, PolySize>::ePolynomialThermo
(
    const dictionary& dict
)
:
    EquationOfState(dict),
    Hf_(dict.subDict("thermodynamics").lookup<scalar>("Hf")),
    Sf_(dict.subDict("thermodynamics").lookup<scalar>("Sf")),
    CvCoeffs_
    (
        dict.subDict("thermodynamics").lookup
        (
            "CvCoeffs<" + Foam::name(PolySize) + '>'
        )
    ),
    eCoeffs_(),
    sCoeffs_(),
    CpCoeffTable_(2)
{
    eCoeffs_ = CvCoeffs_.integral();
    sCoeffs_ = CvCoeffs_.integralMinus1();

    // Offset e poly so that it is relative to the enthalpy at Tstd
    eCoeffs_[0] -= eCoeffs_.value(Tstd);

    // Offset s poly so that it is relative to the entropy at Tstd
    sCoeffs_[0] -= sCoeffs_.value(Tstd);

    List<scalar> lowCoeff(7), highCoeff(7);
    forAll(lowCoeff, i){ lowCoeff[i] = 1.0; highCoeff[i] = 1.0; }
    CpCoeffTable_[0] = lowCoeff; CpCoeffTable_[1] = highCoeff;

}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class EquationOfState, int PolySize>
void Foam::ePolynomialThermo<EquationOfState, PolySize>::write
(
    Ostream& os
) const
{
    EquationOfState::write(os);

    dictionary dict("thermodynamics");
    dict.add("Hf", Hf_);
    dict.add("Sf", Sf_);
    dict.add
    (
        word("CvCoeffs<" + Foam::name(PolySize) + '>'),
        CvCoeffs_
    );
    os  << indent << dict.dictName() << dict;
}


// * * * * * * * * * * * * * * * Ostream Operator  * * * * * * * * * * * * * //

template<class EquationOfState, int PolySize>
Foam::Ostream& Foam::operator<<
(
    Ostream& os,
    const ePolynomialThermo<EquationOfState, PolySize>& pt
)
{
    pt.write(os);
    return os;
}


// ************************************************************************* //
