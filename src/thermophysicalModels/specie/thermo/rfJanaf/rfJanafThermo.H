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

Class
    Foam::rfJanafThermo

Description
    JANAF tables based thermodynamics package templated
    into the equation of state.

SourceFiles
    rfJanafThermoI.H
    rfJanafThermo.C

\*---------------------------------------------------------------------------*/

#ifndef rfJanafThermo_H
#define rfJanafThermo_H

#include "scalar.H"
#include "FixedList.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward declaration of friend functions and operators

template<class EquationOfState> class rfJanafThermo;

template<class EquationOfState>
inline rfJanafThermo<EquationOfState> operator+
(
    const rfJanafThermo<EquationOfState>&,
    const rfJanafThermo<EquationOfState>&
);

template<class EquationOfState>
inline rfJanafThermo<EquationOfState> operator*
(
    const scalar,
    const rfJanafThermo<EquationOfState>&
);

template<class EquationOfState>
inline rfJanafThermo<EquationOfState> operator==
(
    const rfJanafThermo<EquationOfState>&,
    const rfJanafThermo<EquationOfState>&
);

template<class EquationOfState>
Ostream& operator<<
(
    Ostream&,
    const rfJanafThermo<EquationOfState>&
);


/*---------------------------------------------------------------------------*\
                           Class rfJanafThermo Declaration
\*---------------------------------------------------------------------------*/

template<class EquationOfState>
class rfJanafThermo
:
    public EquationOfState
{

public:

    // Public data

        static const int nCoeffs_ = 7;
        typedef FixedList<scalar, nCoeffs_> coeffArray;


private:

    // Private Data

        // Temperature limits of applicability of functions
        scalar Tlow_, Thigh_, Tcommon_;

        coeffArray highCpCoeffs_;
        coeffArray lowCpCoeffs_;

        // Return coefficients array for Cp calculation - Nam
        List<List<scalar>> CpCoeffTable_;

    // Private Member Functions

        //- Check that input data is valid
        void checkInputData() const;

        //- Return the coefficients corresponding to the given temperature
        inline const coeffArray& coeffs(const scalar T) const;


public:

    // Constructors

        //- Construct from components
        inline rfJanafThermo
        (
            const EquationOfState& st,
            const scalar Tlow,
            const scalar Thigh,
            const scalar Tcommon,
            const coeffArray& highCpCoeffs,
            const coeffArray& lowCpCoeffs,
            const bool convertCoeffs = false
        );

        //- Construct from dictionary
        rfJanafThermo(const dictionary& dict);

        //- Construct as a named copy
        inline rfJanafThermo(const word&, const rfJanafThermo&);


    // Member Functions

        //- Return the instantiated type name
        static word typeName()
        {
            return "rfJanaf<" + EquationOfState::typeName() + '>';
        }

        //- Limit the temperature to be in the range Tlow_ to Thigh_
        inline scalar limit(const scalar T) const;


        // Access

            //- Return const access to the low temperature limit
            inline scalar Tlow() const;

            //- Return const access to the high temperature limit
            inline scalar Thigh() const;

            //- Return const access to the common temperature
            inline scalar Tcommon() const;

            //- Return const access to the high temperature poly coefficients
            inline const coeffArray& highCpCoeffs() const;

            //- Return const access to the low temperature poly coefficients
            inline const coeffArray& lowCpCoeffs() const;


        // Fundamental properties

            //- Heat capacity at constant pressure [J/kg/K]
            inline scalar Cp(const scalar p, const scalar T) const;
            inline scalar CpIdeal(const scalar p, const scalar T) const; //Nam

            //- Absolute enthalpy [J/kg]
            inline scalar Ha(const scalar p, const scalar T) const;

            //- Sensible enthalpy [J/kg]
            inline scalar Hs(const scalar p, const scalar T) const;

            //- Derivative of Sensible enthalpy [J/kgK] - Nam
            inline scalar dHsdT(const scalar p, const scalar T) const;

            //- Enthalpy of formation [J/kg]
            inline scalar Hf() const;

            //- Entropy [J/kg/K]
            inline scalar S(const scalar p, const scalar T) const;

            //- Gibbs free energy of the mixture in the standard state [J/kg]
            inline scalar Gstd(const scalar T) const;

            // - Return the coefficient array for Cp calculation - Nam
            inline List<List<scalar>> CpCoeffTable() const
            {
                return CpCoeffTable_;
            }


            #include "HtoEthermo.H"


        // Derivative term used for Jacobian

            //- Temperature derivative of heat capacity at constant pressure
            inline scalar dCpdT(const scalar p, const scalar T) const;


        // I-O

            //- Write to Ostream
            void write(Ostream& os) const;


    // Member Operators

        inline void operator+=(const rfJanafThermo&);


    // Friend operators

        friend rfJanafThermo operator+ <EquationOfState>
        (
            const rfJanafThermo&,
            const rfJanafThermo&
        );

        friend rfJanafThermo operator* <EquationOfState>
        (
            const scalar,
            const rfJanafThermo&
        );

        friend rfJanafThermo operator== <EquationOfState>
        (
            const rfJanafThermo&,
            const rfJanafThermo&
        );


    // Ostream Operator

        friend Ostream& operator<< <EquationOfState>
        (
            Ostream&,
            const rfJanafThermo&
        );
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "rfJanafThermoI.H"

#ifdef NoRepository
    #include "rfJanafThermo.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
