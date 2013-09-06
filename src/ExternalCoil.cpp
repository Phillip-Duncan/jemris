/** @file ExternalCoil.cpp
 *  @brief Implementation of JEMRIS ExternalCoil
 */

/*
 *  JEMRIS Copyright (C) 
 *                        2006-2013  Tony Stoecker
 *                        2007-2013  Kaveh Vahedipour
 *                        2009-2013  Daniel Pflugfelder
 *                                  
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "ExternalCoil.h"
#include "BinaryContext.h"

/***********************************************************/
ExternalCoil::~ExternalCoil () {
}

/***********************************************************/
ExternalCoil* ExternalCoil::Clone() const {
	return (new ExternalCoil(*this));
}

/***********************************************************/
double ExternalCoil::GetSensitivity(const double* position) {

    return InterpolateSensitivity (position);

}

/***********************************************************/
double ExternalCoil::GetPhase(const double* position) {

    return InterpolateSensitivity (position,false);

}

/***********************************************************/
bool ExternalCoil::Prepare (const PrepareMode mode) {

    ATTRIBUTE("Filename" , m_fname);
    ATTRIBUTE("Channel"  , m_channel);

    Coil::Prepare(mode);
	IO::Status ios     = LoadMap();

	return (ios == IO::OK) ? true : false; 

}

/**********************************************************/
IO::Status ExternalCoil::LoadMap () {

	BinaryContext bc;
	DataInfo      di;
	IO::Status   ios = IO::OK;
	std::vector<double> tmpdat;

	bc.Initialize (m_fname, IO::IN);
	if (bc.Status() != IO::OK)
		return bc.Status();

	if (bc.ReadData<double>(tmpdat, "maps", "magnitude") != IO::OK)
		return bc.Status();

	// no 'int pow(int,int)' available! Use cast and add delta to avoid roundoff error.
	int size = (int) (pow((double) m_points,(double) m_dim) + 1e-20);
	int pos  = m_channel*size;

	memcpy (&(m_sens_mag[0][0][0]), &tmpdat[pos], size * sizeof(double));

	if (bc.ReadData<double>(tmpdat, "maps", "phase") != IO::OK)
		return bc.Status();

	memcpy (&(m_sens_pha[0][0][0]), &tmpdat[pos], size * sizeof(double));

	return ios;

}

