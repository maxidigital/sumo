#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2019 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    test.py
# @author  Pablo Alvarez Lopez
# @date    2016-11-25
# @version $Id$

# import common functions for netedit tests
import os
import sys

testRoot = os.path.join(os.environ.get('SUMO_HOME', '.'), 'tests')
neteditTestRoot = os.path.join(
    os.environ.get('TEXTTEST_HOME', testRoot), 'netedit')
sys.path.append(neteditTestRoot)
import neteditTestFunctions as netedit  # noqa

# Open netedit
neteditProcess, referencePosition = netedit.setupAndStart(neteditTestRoot)

# go to additional mode
netedit.additionalMode()

# select parkingArea
netedit.changeAdditional("parkingArea")

# create parkingArea in mode "reference left"
netedit.leftClick(referencePosition, 250, 250)

# select space
netedit.changeAdditional("space")

# select parent
netedit.selectAdditionalChild(8, 0)

# create space
netedit.leftClick(referencePosition, 250, 110)

# change to move mode
netedit.moveMode()

# move space
netedit.moveElement(referencePosition, 100, 20, 330, 100)

# go to inspect mode
netedit.inspectMode()

# inspect parkingArea
netedit.leftClick(referencePosition, 410, 130)

# block space
netedit.modifyBoolAttribute(8)

# change to move mode
netedit.moveMode()

# try to move space
netedit.moveElement(referencePosition, 340, 100, 100, 40)

# go to inspect mode
netedit.inspectMode()

# inspect parkingArea
netedit.leftClick(referencePosition, 410, 120)

# unblock space
netedit.modifyBoolAttribute(8)

# change to move mode
netedit.moveMode()

# move space
netedit.moveElement(referencePosition, 340, 100, 100, 40)

# Check undos and redos
netedit.undo(referencePosition, 6)
netedit.redo(referencePosition, 6)

# save additionals
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
