/****************************************************************
 * file guifox_rps.cc
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Description:
 *      This file is part of the Reflective Persistent System.
 *
 *      It has the initial graphical user interface using FOX toolkit 1.7.80
 *      (see fox-toolkit.org)
 *
 * Author(s):
 *      Basile Starynkevitch <basile@starynkevitch.net>
 *      Abhishek Chakravarti <abhishek@taranjali.org>
 *      Nimesh Neema <nimeshneema@gmail.com>
 *
 *      © Copyright 2022 The Reflective Persistent System Team
 *      team@refpersys.org & http://refpersys.org/
 *
 * License:
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/


#include "refpersys.hh"

#include "fx.h"

bool rps_fox_gui;

std::vector<const char*> fox_args_vect_rps;


void
add_fox_arg_rps(char*arg)
{
  if (fox_args_vect_rps.empty())
    fox_args_vect_rps.push_back(rps_progname);
  if (arg)
    fox_args_vect_rps.push_back(arg);
} // end add_fox_arg_rps

void
guifox_initialize_rps(void)
{
  RPS_FATAL("unimplemented guifox_initialize_rps");
#warning unimplemented guifox_initialize_rps
} // end guifox_initialize_rps


void
guifox_run_application_rps(void)
{
  RPS_FATAL("unimplemented guifox_run_application_rps");
#warning unimplemented guifox_run_application_rps
} // end of guifox_run_application_rps

//// end of file guifox_rps.cc
