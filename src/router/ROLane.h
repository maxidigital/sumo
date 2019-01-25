/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    ROLane.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @date    Sept 2002
/// @version $Id$
///
// A single lane the router may use
/****************************************************************************/
#ifndef ROLane_h
#define ROLane_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <vector>
#include <utils/common/Named.h>
#include <utils/common/SUMOVehicleClass.h>


// ===========================================================================
// class declarations
// ===========================================================================
class ROEdge;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROLane
 * @brief A single lane the router may use
 *
 * Currently, the lane has no other purpose then storing the allowed vehicle
 *  classes. They are even only stored herein and used by computing the vehicle
 *  classes allowed on the according edge.
 * @see ROEdge
 */
class ROLane : public Named {
public:
    /** @brief Constructor
     *
     * @param[in] id The id of the lane
     * @param[in] length The length of the lane
     * @param[in] maxSpeed The maximum speed allowed on the lane
     * @param[in] permissions Vehicle classes that may pass this lane
     */
    ROLane(const std::string& id, ROEdge* edge, double length, double maxSpeed, SVCPermissions permissions) :
        Named(id), myEdge(edge), myLength(length), myMaxSpeed(maxSpeed), myPermissions(permissions) {
    }


    /// @brief Destructor
    ~ROLane() { }


    /** @brief Returns the length of the lane
     * @return The length of this lane
     */
    double getLength() const {
        return myLength;
    }


    /** @brief Returns the maximum speed allowed on this lane
     * @return The maximum speed allowed on this lane
     */
    double getSpeed() const {
        return myMaxSpeed;
    }


    /** @brief Returns the list of allowed vehicle classes
     * @return The list of vehicle classes allowed on this lane
     */
    inline SVCPermissions getPermissions() const {
        return myPermissions;
    }

    /** @brief Returns the lane's edge
     * @return This lane's edge
     */
    ROEdge& getEdge() const {
        return *myEdge;
    }

    /// @brief get the map of outgoing lanes to via edges
    const std::vector<std::pair<const ROLane*, const ROEdge*> >& getOutgoingViaLanes() const {
        return myOutgoingLanes;
    }

    void addOutgoingLane(ROLane* lane, ROEdge* via = nullptr) {
        myOutgoingLanes.push_back(std::make_pair(lane, via));
    }

    /// @brief get the state of the link from the logical predecessor to this lane (ignored for routing)
    inline LinkState getIncomingLinkState() const {
        return LINKSTATE_MAJOR;
    }

    inline bool allowsVehicleClass(SUMOVehicleClass vclass) const {
        return (myPermissions & vclass) == vclass;
    }

private:
    /// @brief The parent edge of this lane
    ROEdge* myEdge;

    /// @brief The length of the lane
    double myLength;

    /// @brief The maximum speed allowed on the lane
    double myMaxSpeed;

    /// @brief The encoding of allowed vehicle classes
    SVCPermissions myPermissions;

    std::vector<std::pair<const ROLane*, const ROEdge*> > myOutgoingLanes;


private:
    /// @brief Invalidated copy constructor
    ROLane(const ROLane& src);

    /// @brief Invalidated assignment operator
    ROLane& operator=(const ROLane& src);

};


#endif

/****************************************************************************/

