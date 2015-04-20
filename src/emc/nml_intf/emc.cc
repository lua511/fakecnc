/********************************************************************
* Description: emc.cc
*
* Author: winter
* License: GPL Version 2
* System: Linux,edited by ms vs on windows
* Last change:
********************************************************************/
// Include all NML, CMS, and RCS classes and functions
#include "rcs.hh"

// Include command and status message definitions
#include "canon.hh"
#include "canon_position.hh"
#include "emc.hh"
#include "emc_nml.hh"
#include "emcglb.h"
#include "emcpos.h"
#include "cms.hh"
#include "notimplement.hh"


int emcFormat(NMLTYPE type, void *buffer, CMS * cms)
{
	ReportNotimplement::report("emc::emcFormat");
	return 1;
}

const char *emc_symbol_lookup(uint32_t type)
{
	ReportNotimplement::report("emc::emc_symbol_ookup");
    return (NULL);
}
