// -*- mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
// vi: set et ts=4 sw=4 sts=4:
/*
  Copyright (C) 2009-2013 by Andreas Lauser

  This file is part of the Open Porous Media project (OPM).

  OPM is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  OPM is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with OPM.  If not, see <http://www.gnu.org/licenses/>.
*/
/*!
 * \file
 *
 * \copydoc Ewoms::FlashFluxVariables
 */
#ifndef EWOMS_FLASH_FLUX_VARIABLES_HH
#define EWOMS_FLASH_FLUX_VARIABLES_HH

#include "flashproperties.hh"

#include <ewoms/models/common/multiphasebasefluxvariables.hh>
#include <ewoms/models/common/energymodule.hh>
#include <ewoms/models/common/diffusionmodule.hh>

namespace Ewoms {

/*!
 * \ingroup FlashModel
 * \ingroup FluxVariables
 *
 * \brief This template class contains the data which is required to
 *        calculate all fluxes of components over a face of a finite
 *        volume for the compositional multi-phase model based on
 *        flash calculations.
 *
 * This means pressure and concentration gradients, phase densities at
 * the integration point, etc.
 */
template <class TypeTag>
class FlashFluxVariables
    : public MultiPhaseBaseFluxVariables<TypeTag>
    , public EnergyFluxVariables<TypeTag, GET_PROP_VALUE(TypeTag, EnableEnergy)>
    , public DiffusionFluxVariables<TypeTag, GET_PROP_VALUE(TypeTag, EnableDiffusion)>
{
    typedef MultiPhaseBaseFluxVariables<TypeTag> ParentType;

    typedef typename GET_PROP_TYPE(TypeTag, ElementContext) ElementContext;
    typedef typename GET_PROP_TYPE(TypeTag, FluidSystem) FluidSystem;

    enum { enableDiffusion = GET_PROP_VALUE(TypeTag, EnableDiffusion) };
    typedef Ewoms::DiffusionFluxVariables<TypeTag, enableDiffusion> DiffusionFluxVariables;

    enum { enableEnergy = GET_PROP_VALUE(TypeTag, EnableEnergy) };
    typedef Ewoms::EnergyFluxVariables<TypeTag, enableEnergy> EnergyFluxVariables;

public:
    /*!
     * \copydoc MultiPhaseBaseFluxVariables::update
     */
    void update(const ElementContext &elemCtx, int scvfIdx, int timeIdx)
    {
        ParentType::update(elemCtx, scvfIdx, timeIdx);
        DiffusionFluxVariables::update_(elemCtx, scvfIdx, timeIdx);
        EnergyFluxVariables::update_(elemCtx, scvfIdx, timeIdx);
    }

    /*!
     * \copydoc MultiPhaseBaseFluxVariables::updateBoundary
     */
    template <class Context, class FluidState>
    void updateBoundary(const Context &context, int bfIdx, int timeIdx,
                        const FluidState &fluidState,
                        typename FluidSystem::ParameterCache &paramCache)
    {
        ParentType::updateBoundary(context, bfIdx, timeIdx,
                                   fluidState, paramCache);
        DiffusionFluxVariables::updateBoundary_(context, bfIdx, timeIdx,
                                                fluidState);
        EnergyFluxVariables::updateBoundary_(context, bfIdx, timeIdx, fluidState);
    }
};

} // namespace Ewoms

#endif
