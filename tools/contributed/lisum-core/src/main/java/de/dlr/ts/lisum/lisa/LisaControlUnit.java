/** ************************************************************************* */
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2016-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/** ************************************************************************* */
/// @file    Constants.java
/// @author  Maximiliano Bottazzi
/// @date    2016
/// @version $Id$
///
//
/** ************************************************************************* */
package de.dlr.ts.lisum.lisa;

import de.dlr.ts.commons.logger.DLRLogger;
import de.dlr.ts.lisum.exceptions.LisaRESTfulServerNotFoundException;
import de.dlr.ts.lisum.enums.LightColor;
import de.dlr.ts.lisum.interfaces.ControlUnitInterface;
import de.dlr.ts.lisum.interfaces.DetectorInterface;
import de.dlr.ts.lisum.interfaces.SignalProgramInterface;
import java.io.File;
import java.util.ArrayList;
import java.util.List;

/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano
 * Bottazzi</a>
 */
final class LisaControlUnit implements ControlUnitInterface {

    private int zNr;
    private int fNr;
    private String fullName;

    private final LisaSignalGroups lisaSignalGroups = new LisaSignalGroups();
    private final LisaDetectors lisaDetectors = new LisaDetectors();
    private final LisaSignalPrograms signalPrograms = new LisaSignalPrograms();

    private Message message;
    private final WunschVector vector;

    private int currentTask;
    private LisaCommands commands;

    private Persistence persistence;
    private boolean enabled = false;

    private final List<String> apWerteNames = new ArrayList<>();
    private List<String> apWerteValues = new ArrayList<>();

    
    /**
     *
     */
    public LisaControlUnit() {
        vector = new WunschVector(signalPrograms);
    }
    
    @Override
    public int apWerteCount() {
        return apWerteNames.size();
    }
    
    @Override
    public String getAPWerteName(int index) {
        if(apWerteNames.isEmpty())
            return "";
        
        return apWerteNames.get(index);
    }

    @Override
    public String getAPWerteValue(int index) {
        if(apWerteValues == null || apWerteValues.isEmpty())
            return "";
        
        return apWerteValues.get(index);
    }

    /**
     *
     * @param commands
     */
    public void setCommands(LisaCommands commands) {
        this.commands = commands;
    }

    /**
     *
     * @param lisaDirectory
     */
    public void setLisaDirectory(File lisaDirectory) {
        persistence = new Persistence(vector, lisaDirectory, this);
    }

    /**
     *
     * @param conf
     */
    void load(LisaConfigurationFiles.ControlUnit cu) {
        DLRLogger.config(this, "Loading Control Unit " + cu.fullName);

        this.zNr = cu.zNr;
        this.fNr = cu.fNr;
        this.fullName = cu.fullName;

        lisaSignalGroups.load(cu);
        lisaDetectors.load(cu);
        signalPrograms.load(cu);
        
        for (String na : cu.apWerteNames)
            this.apWerteNames.add(na);        
    }

    /**
     *
     * @param name
     * @return
     */
    @Override
    public DetectorInterface getDetector(String name) {
        return this.lisaDetectors.getDetector(name);
    }

    /**
     * Not used
     */
    private String getObjectList() throws LisaRESTfulServerNotFoundException {
        return commands.getObjectList();
    }

    /**
     *
     */
    public void initBeforePlay() throws LisaRESTfulServerNotFoundException {
        persistence.readPersistence();

        String objectList = getObjectList(); //ignore value

        removeAllOldTaskLists();
        currentTask = commands.setTask(zNr, fNr, 60 /* interval */);

        /**
         *
         */
        message = new Message(currentTask);
        message.intervall = 60;

        /**
         *
         */
        String command = message.getCommand(Message.Type.Init, vector, null, 0l);
        commands.putMessage(command);
    }

    /**
     *
     */
    public void executeSimulationStep(long simulationTime) throws LisaRESTfulServerNotFoundException {
        if (enabled) {
            String detectorsString = lisaDetectors.getLisaString();

            //Preparing message to send
            String commandToLisa = message.getCommand(Message.Type.Run, vector, detectorsString, simulationTime);

            //Sending message to Lisa and receiving a response
            PutMessageResponse messageResponseFromLisa = commands.putMessage(commandToLisa);

            try {
                lisaSignalGroups.parseStates(messageResponseFromLisa.getSignalsStateVector()); //Catch NULL value
            } catch (NullPointerException e) {
            }

            String apWerte = messageResponseFromLisa.getApWertZustType();                        
            apWerteValues = APWertZustType.parse(apWerte);            
        }
    }

    /**
     *
     */
    private void removeAllOldTaskLists() throws LisaRESTfulServerNotFoundException {
        int[] taskList = commands.getTaskList(zNr, fNr);

        for (int i : taskList) {
            commands.removeTaskList(i);
        }
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder("Control Unit: ZNr ").append(zNr)
                .append(" FNr ").append(fNr).append(" ");

        sb.append(lisaSignalGroups.toString());

        return sb.toString();
    }

    /**
     *
     * @return
     */
    public LisaSignalGroups getLisaSignalGroups() {
        return lisaSignalGroups;
    }

    @Override
    public final String getFullName() {
        return fullName;
    }

    @Override
    public int signalGroupsCount() {
        return lisaSignalGroups.signalsCount();
    }

    @Override
    public SignalProgramInterface[] getSignalPrograms() {
        return signalPrograms.getSignalProgramsArray();
    }

    @Override
    public void setCurrentSignalProgram(int index) {
        signalPrograms.setCurrentSignalProgram(index);

        if (index == 0) {
            vector.setKnotenEinAus(2);
        } else {
            vector.setKnotenEinAus(1);
        }
    }

    @Override
    public boolean isVA() {
        return vector.isVA();
    }

    @Override
    public void setVA(boolean va) {
        vector.setVA(va);
    }

    @Override
    public boolean isIV() {
        return vector.isIV();
    }

    @Override
    public void setIV(boolean iv) {
        vector.setIV(iv);
    }

    @Override
    public boolean isOV() {
        return vector.isOV();
    }

    @Override
    public void setOV(boolean ov) {
        vector.setOV(ov);
    }

    @Override
    public SignalProgramInterface getCurrentSignalProgram() {
        return this.signalPrograms.getCurrentSignalProgram();
    }

    @Override
    public int getEbene() {
        return vector.getEbene();
    }

    @Override
    public void setEbene(int ebene) {
        vector.setEbene(ebene);
    }

    @Override
    public int getKnotenEinAus() {
        return vector.getKnotenEinAus();
    }

    @Override
    public void setKnotenEinAus(int einAus) {
        vector.setKnotenEinAus(einAus);
    }

    @Override
    public void storePersistent() {
        persistence.store();
    }

    @Override
    public LightColor getLightColor(int signalGroupIndex) {
        return lisaSignalGroups.getColor(signalGroupIndex);
    }

    @Override
    public LightColor getLightColor(String signalGroupName) {
        return lisaSignalGroups.getColor(signalGroupName);
    }

    @Override
    public void setEnabled(boolean enabled) {
        this.enabled = enabled;
    }

    @Override
    public boolean isEnabled() {
        return enabled;
    }

    @Override
    public void setCoordinated(int coordinated) {
        vector.setCoordinated(coordinated);
    }

    @Override
    public int getCoordinated() {
        return vector.getCoordinated();
    }

}
