/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2002-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUILoadThread.h
/// @author  Daniel Krajzewicz
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Sept 2002
/// @version $Id$
///
// Class describing the thread that performs the loading of a simulation
/****************************************************************************/
#ifndef GUILoadThread_h
#define GUILoadThread_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/common/SUMOTime.h>
#include <utils/common/MsgHandler.h>
#include <utils/foxtools/FXSingleEventThread.h>
#include <utils/foxtools/FXThreadEvent.h>
#include <utils/foxtools/MFXEventQue.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUIApplicationWindow;
class GUINet;
class GUIEvent;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUILoadThread
 */
class GUILoadThread : public FXSingleEventThread {
public:
    /// constructor
    GUILoadThread(FXApp* app, GUIApplicationWindow* mw, MFXEventQue<GUIEvent*>& eq,
                  FXEX::FXThreadEvent& ev);

    /// destructor
    virtual ~GUILoadThread();

    /** starts the thread
        the thread ends after the net has been loaded */
    FXint run();

    /// begins the loading of the given file
    void loadConfigOrNet(const std::string& file, bool isNet);

    /// Retrieves messages from the loading module
    void retrieveMessage(const MsgHandler::MsgType type, const std::string& msg);

    const std::string& getFileName() const;

protected:
    /** @brief Closes the loading process
     *
     * This method is called both on success and failure.
     * All message callbacks to this instance are removed and the parent
     * application is informed about the loading */
    void submitEndAndCleanup(GUINet* net, const SUMOTime simStartTime, const SUMOTime simEndTime,
                             const std::vector<std::string>& guiSettingsFiles = std::vector<std::string>(),
                             const bool osgView = false,
                             const bool viewportFromRegistry = false);

protected:
    /// the parent window to inform about the loading
    GUIApplicationWindow* myParent;

    /// the path to load the simulation from
    std::string myFile;

    /// the title string for the application
    std::string myTitle;

    /** @brief The instances of message retriever encapsulations
        Needed to be deleted from the handler later on */
    OutputDevice* myErrorRetriever, *myMessageRetriever, *myWarningRetriever;

    MFXEventQue<GUIEvent*>& myEventQue;

    FXEX::FXThreadEvent& myEventThrow;

    /// Information whether only the network shall be loaded
    bool myLoadNet;

};


#endif

/****************************************************************************/
