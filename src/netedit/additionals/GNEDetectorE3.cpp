/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDetectorE3.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
//
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================

#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/netelements/GNEEdge.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/images/GUITextureSubSys.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNEDetectorE3.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEDetectorE3::GNEDetectorE3(const std::string& id, GNEViewNet* viewNet, Position pos, double freq, const std::string& filename, const std::string& vehicleTypes, const std::string& name, const double timeThreshold, double speedThreshold, bool blockMovement) :
    GNEAdditional(id, viewNet, GLO_E3DETECTOR, SUMO_TAG_E3DETECTOR, name, blockMovement),
    myPosition(pos),
    myFreq(freq),
    myFilename(filename),
    myVehicleTypes(vehicleTypes),
    myTimeThreshold(timeThreshold),
    mySpeedThreshold(speedThreshold) {
}


GNEDetectorE3::~GNEDetectorE3() {}


void
GNEDetectorE3::updateGeometry(bool updateGrid) {
    // first check if object has to be removed from grid (SUMOTree)
    if (updateGrid) {
        myViewNet->getNet()->removeGLObjectFromGrid(this);
    }

    // Clear shape
    myGeometry.shape.clear();

    // Set block icon position
    myBlockIcon.position = myPosition;

    // Set block icon offset
    myBlockIcon.offset = Position(-0.5, -0.5);

    // Set block icon rotation, and using their rotation for draw logo
    myBlockIcon.setRotation();

    // Set position
    myGeometry.shape.push_back(myPosition);

    // Update connection's geometry
    myChildConnections.update();

    // last step is to check if object has to be added into grid (SUMOTree) again
    if (updateGrid) {
        myViewNet->getNet()->addGLObjectIntoGrid(this);
    }
}


Position
GNEDetectorE3::getPositionInView() const {
    return myPosition;
}


void
GNEDetectorE3::moveGeometry(const Position& offset) {
    // restore old position, apply offset and update Geometry
    myPosition = myMove.originalViewPosition;
    myPosition.add(offset);
    // filtern position using snap to active grid
    // filtern position using snap to active grid
    myPosition = myViewNet->snapToActiveGrid(myPosition);
    updateGeometry(false);
}


void
GNEDetectorE3::commitGeometryMoving(GNEUndoList* undoList) {
    // commit new position allowing undo/redo
    undoList->p_begin("position of " + getTagStr());
    undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_POSITION, toString(myPosition), true, toString(myMove.originalViewPosition)));
    undoList->p_end();
}


std::string
GNEDetectorE3::getParentName() const {
    return myViewNet->getNet()->getMicrosimID();
}


void
GNEDetectorE3::drawGL(const GUIVisualizationSettings& s) const {
    // Start drawing adding an gl identificator
    glPushName(getGlID());

    // Add a draw matrix for drawing logo
    glPushMatrix();
    glTranslated(myGeometry.shape[0].x(), myGeometry.shape[0].y(), getType());

    // Draw icon depending of detector is selected and if isn't being drawn for selecting
    if (s.drawForSelecting) {
        GLHelper::setColor(RGBColor::GREY);
        GLHelper::drawBoxLine(Position(0, 1), 0, 2, 1);
    } else {
        glColor3d(1, 1, 1);
        glRotated(180, 0, 0, 1);
        if (isAttributeCarrierSelected()) {
            GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_E3SELECTED), 1);
        } else {
            GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GNETEXTURE_E3), 1);
        }
    }

    // Pop logo matrix
    glPopMatrix();
    if (!s.drawForSelecting) {
        // Show Lock icon depending of the Edit mode
        myBlockIcon.draw(0.4);
        // Draw connections
        myChildConnections.draw();
    }
    // Draw name if isn't being drawn for selecting
    if (!s.drawForSelecting) {
        drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
    }
    // check if dotted contour has to be drawn
    if (!s.drawForSelecting && (myViewNet->getDottedAC() == this)) {
        GLHelper::drawShapeDottedContour(getType(), myPosition, 2, 2);
        // draw shape dotte contour aroud alld connections between child and parents
        for (auto i : myChildConnections.connectionPositions) {
            GLHelper::drawShapeDottedContour(getType(), i, 0);
        }
    }
    // Pop name
    glPopName();
}


std::string
GNEDetectorE3::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getAdditionalID();
        case SUMO_ATTR_POSITION:
            return toString(myPosition);
        case SUMO_ATTR_FREQUENCY:
            return toString(myFreq);
        case SUMO_ATTR_NAME:
            return myAdditionalName;
        case SUMO_ATTR_FILE:
            return myFilename;
        case SUMO_ATTR_VTYPES:
            return myVehicleTypes;
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
            return toString(myTimeThreshold);
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
            return toString(mySpeedThreshold);
        case GNE_ATTR_BLOCK_MOVEMENT:
            return toString(myBlockMovement);
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_GENERIC:
            return getGenericParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEDetectorE3::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID: {
            // change ID of Entry
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            // Change Ids of all Entry/Exits childs
            for (auto i : myAdditionalChilds) {
                i->setAttribute(SUMO_ATTR_ID, generateAdditionalChildID(i->getTagProperty().getTag()), undoList);
            }
            break;
        }
        case SUMO_ATTR_FREQUENCY:
        case SUMO_ATTR_POSITION:
        case SUMO_ATTR_NAME:
        case SUMO_ATTR_FILE:
        case SUMO_ATTR_VTYPES:
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
        case GNE_ATTR_BLOCK_MOVEMENT:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_GENERIC:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEDetectorE3::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidDetectorID(value);
        case SUMO_ATTR_POSITION:
            return canParse<Position>(value);
        case SUMO_ATTR_FREQUENCY:
            return canParse<double>(value) && (parse<double>(value) >= 0);
        case SUMO_ATTR_NAME:
            return SUMOXMLDefinitions::isValidAttribute(value);
        case SUMO_ATTR_FILE:
            return SUMOXMLDefinitions::isValidFilename(value);
        case SUMO_ATTR_VTYPES:
            if (value.empty()) {
                return true;
            } else {
                return SUMOXMLDefinitions::isValidListOfTypeID(value);
            }
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
            return canParse<double>(value) && (parse<double>(value) >= 0);
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
            return canParse<double>(value) && (parse<double>(value) >= 0);
        case GNE_ATTR_BLOCK_MOVEMENT:
            return canParse<bool>(value);
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_GENERIC:
            return isGenericParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEDetectorE3::checkAdditionalChildRestriction() const {
    int numEntrys = 0;
    int numExits = 0;
    // iterate over additional chidls and obtain number of entrys and exits
    for (auto i : myAdditionalChilds) {
        if (i->getTagProperty().getTag() == SUMO_TAG_DET_ENTRY) {
            numEntrys++;
        } else if (i->getTagProperty().getTag() == SUMO_TAG_DET_EXIT) {
            numExits++;
        }
    }
    // write warnings
    if (numEntrys == 0) {
        WRITE_WARNING("An " + toString(SUMO_TAG_E3DETECTOR) + " need at least one " + toString(SUMO_TAG_DET_ENTRY) + " detector");
    }
    if (numExits == 0) {
        WRITE_WARNING("An " + toString(SUMO_TAG_E3DETECTOR) + " need at least one " + toString(SUMO_TAG_DET_EXIT) + " detector");
    }
    // return false depending of number of Entrys and Exits
    return ((numEntrys != 0) && (numExits != 0));
}


std::string
GNEDetectorE3::getPopUpID() const {
    return getTagStr() + ":" + getID();
}


std::string
GNEDetectorE3::getHierarchyName() const {
    return getTagStr();
}

// ===========================================================================
// private
// ===========================================================================

void
GNEDetectorE3::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            changeAdditionalID(value);
            break;
        case SUMO_ATTR_POSITION:
            myPosition = parse<Position>(value);
            break;
        case SUMO_ATTR_FREQUENCY:
            myFreq = parse<double>(value);
            break;
        case SUMO_ATTR_NAME:
            myAdditionalName = value;
            break;
        case SUMO_ATTR_FILE:
            myFilename = value;
            break;
        case SUMO_ATTR_VTYPES:
            myVehicleTypes = value;
            break;
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
            myTimeThreshold = parse<double>(value);
            break;
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
            mySpeedThreshold = parse<double>(value);
            break;
        case GNE_ATTR_BLOCK_MOVEMENT:
            myBlockMovement = parse<bool>(value);
            break;
        case GNE_ATTR_SELECTED:
            if (parse<bool>(value)) {
                selectAttributeCarrier();
            } else {
                unselectAttributeCarrier();
            }
            break;
        case GNE_ATTR_GENERIC:
            setGenericParametersStr(value);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
    // Update Geometry after setting a new attribute (but avoided for certain attributes)
    if((key != SUMO_ATTR_ID) && (key != GNE_ATTR_GENERIC) && (key != GNE_ATTR_SELECTED)) {
        updateGeometry(true);
    }
}

/****************************************************************************/
