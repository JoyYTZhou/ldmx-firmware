#!/usr/bin/env python3

import os
import pyrogue as pr

top_level = os.path.realpath(__file__).split('software')[0]

pr.addLibraryPath(top_level+'firmware/submodules/surf/python')
pr.addLibraryPath(top_level+'firmware/submodules/axi-soc-ultra-plus-core/python')
pr.addLibraryPath(top_level+'firmware/common/ts/python')

import ldmx_ts

import sys
import argparse
import importlib
import rogue

import pyrogue.pydm

if __name__ == "__main__":

#################################################################

    # Set the argument parser
    parser = argparse.ArgumentParser()

    # Convert str to bool
    argBool = lambda s: s.lower() in ['true', 't', 'yes', '1']

    # Add arguments
    parser.add_argument(
        "--ip",
        type     = str,
        required = True,
        help     = "ETH Host Name (or IP address)",
    )

    # Get the arguments
    args = parser.parse_args()

    #################################################################

    with ldmx_ts.ZccmRoot(
        ip       = args.ip,
        top_level=top_level,
        zmqSrvEn = True,
    ) as root:
        pyrogue.pydm.runPyDM(
            serverList = root.zmqServer.address,
            sizeX      = 800,
            sizeY      = 800,
        )

    #################################################################
