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

#include "makeReaction.H"

#include "LangmuirHinshelwoodReactionRate.H"

#include "forCommonGases.H"
//#include "forKineticGases.H" //
#include "forSRKchungTakaGases.H" //
#include "forPRchungTakaGases.H" //
#include "forSRKchungKineticGases.H" //
#include "forPRchungKineticGases.H" //
#include "forIdKineticGases.H" //
#include "forCommonLiquids.H"
#include "forPolynomials.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
    //forKineticGases(makeIRReactions, LangmuirHinshelwoodReactionRate); //
    forSRKchungTakaGases(makeIRReactions, LangmuirHinshelwoodReactionRate); //
    forPRchungTakaGases(makeIRReactions, LangmuirHinshelwoodReactionRate); //
    forSRKchungKineticGases(makeIRReactions, LangmuirHinshelwoodReactionRate); //
    forPRchungKineticGases(makeIRReactions, LangmuirHinshelwoodReactionRate); //
    forIdKineticGases(makeIRReactions, LangmuirHinshelwoodReactionRate); //
    forCommonGases(makeIRReactions, LangmuirHinshelwoodReactionRate);
    forCommonLiquids(makeIRReactions, LangmuirHinshelwoodReactionRate);
    forPolynomials(makeIRReactions, LangmuirHinshelwoodReactionRate);
}

// ************************************************************************* //
