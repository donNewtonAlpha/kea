// Copyright (C) 2014-2017 Internet Systems Consortium, Inc. ("ISC")
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef HOST_RESERVATIONS_LIST_PARSER_H
#define HOST_RESERVATIONS_LIST_PARSER_H

#include <cc/data.h>
#include <cc/simple_parser.h>
#include <dhcpsrv/subnet_id.h>
#include <boost/foreach.hpp>

namespace isc {
namespace dhcp {

/// @brief Parser for a list of host reservations for a subnet.
///
/// @tparam HostReservationParserType Host reservation parser to be used to
/// parse individual reservations: @c HostReservationParser4 or
/// @c HostReservationParser6.
template<typename HostReservationParserType>
class HostReservationsListParser : public isc::data::SimpleParser {
public:

    /// @brief Parses a list of host reservation entries for a subnet.
    ///
    /// @param subnet_id Identifier of the subnet to which the reservations
    /// belong.
    /// @param hr_list Data element holding a list of host reservations.
    /// Each host reservation is described by a map object.
    ///
    /// @throw DhcpConfigError If the configuration if any of the reservations
    /// is invalid.
    void parse(const SubnetID& subnet_id, isc::data::ConstElementPtr hr_list) {
        BOOST_FOREACH(data::ConstElementPtr reservation, hr_list->listValue()) {
            HostReservationParserType parser;
            parser.parse(subnet_id, reservation);
        }
    }
};

}
}

#endif // HOST_RESERVATIONS_LIST_PARSER_H
