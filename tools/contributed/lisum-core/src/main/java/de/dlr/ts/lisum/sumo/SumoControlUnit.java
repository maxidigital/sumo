/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2016-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    Constants.java
/// @author  Maximiliano Bottazzi
/// @date    2016
/// @version $Id$
///
//
/****************************************************************************/
package de.dlr.ts.lisum.sumo;

import de.dlr.ts.commons.logger.DLRLogger;
import de.dlr.ts.lisum.enums.LightColor;
import de.dlr.ts.lisum.interfaces.ControlUnitInterface;
import de.dlr.ts.lisum.simulation.SimulationControlUnits;
import de.tudresden.sumo.cmd.Trafficlight;
import it.polito.appeal.traci.SumoTraciConnection;
import java.util.ArrayList;
import java.util.List;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
class SumoControlUnit 
{
    private final List<SignalGroup> signalGroups = new ArrayList<>();
    private final String name;    
    private SimulationControlUnits controlUnits;    
    private SumoTraciConnection sumoTraciConnection;

    /**
     *
     */
    public SumoControlUnit(String name, String[] sumoStrings) {
        this.name = name;
        int phasesCount = sumoStrings[0].length();

        for (int i = 0; i < phasesCount; i++) {
            SignalGroup sg = new SignalGroup();
            signalGroups.add(sg);
        }
    }

    public void setSumoTraciConnection(SumoTraciConnection sumoTraciConnection) {
        this.sumoTraciConnection = sumoTraciConnection;
    }

    /**
     *
     * @param index
     * @param lines
     * @return
     */
    private String extractStates(int index, String[] lines) {
        String tmp = "";

        for (String line : lines) {
            tmp += line.charAt(index);
        }

        return tmp;
    }

    /**
     *
     * @param controlUnits
     */
    void setControlUnits(SimulationControlUnits controlUnits) {
        this.controlUnits = controlUnits;
    }

    /**
     *
     */
    public void executeSimulationStep() 
    {
        for (int j = 0; j < signalGroups.size(); j++)
            signalGroups.get(j).setCurrentLightColor(controlUnits.getLightColor(this.name, j));        
        
        set();
        
        /**
         * Setting APWerte
         */
        
        //DLRLogger.fine(this, "Getting control unit " + this.name);
        ControlUnitInterface cu = controlUnits.getControlUnitInterfaceBySumoName(this.name);
        
        if(cu == null)
            return;                       
        
        for (int i = 0; i < cu.apWerteCount(); i++) {                                    
            String apwertName = cu.getAPWerteName(i);            
            String apwertValue = cu.getAPWerteValue(i);            
            
            if(apwertName.isEmpty())
                continue;
            
            DLRLogger.finest("Control unit " + name + ": APwerte: " + apwertName + "  " + apwertValue);            
            try {                        
                sumoTraciConnection.do_job_set(Trafficlight.setParameter(name, apwertName, apwertValue));
            } catch (Exception ex) {
                Logger.getLogger(SumoControlUnit.class.getName()).log(Level.SEVERE, null, ex);
            }
        }                
    }

    /**
     *
     */
    private void set() {
        String signalGroupState = "";

        for (SignalGroup signalGroup : signalGroups) {
            char sumoCurrentLightColor = signalGroup.getSumoCurrentLightColor();
            signalGroupState += sumoCurrentLightColor;
        }

        try {
            //trafficlight.
            sumoTraciConnection.do_job_set(Trafficlight.setRedYellowGreenState(name, signalGroupState));

            //trafficLight.changeLightsState(new TLState(signalGroupState));
        } catch (IOException ex) {
            ex.printStackTrace(System.out);
        } catch (Exception ex) {
            Logger.getLogger(SumoControlUnit.class.getName()).log(Level.SEVERE, null, ex);
        }
    }

    /**
     *
     * @return
     */
    public String getName() {
        return name;
    }

    /**
     *
     * @return
     */
    public int getSignalGroupsCount() {
        return signalGroups.size();
    }

    /**
     *
     * @param signalGroupIndex
     * @param color
     */
    public void setLightColor(int signalGroupIndex, LightColor color) {
        signalGroups.get(signalGroupIndex).setCurrentLightColor(color);
    }

    /**
     *
     */
    public static class SignalGroup {

        private LightColor currentLightColor = LightColor.OFF;

        /**
         *
         */
        public SignalGroup() {
        }

        /**
         *
         * @param currentLightColor
         */
        public void setCurrentLightColor(LightColor currentLightColor) {
            this.currentLightColor = currentLightColor;
        }

        /**
         *
         * @return
         */
        public char getSumoCurrentLightColor() {
            return currentLightColor.getSumoCode();
        }
    }

}
