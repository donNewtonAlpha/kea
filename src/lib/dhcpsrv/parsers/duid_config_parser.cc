// Copyright (C) 2015,2017 Internet Systems Consortium, Inc. ("ISC")
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <config.h>
#include <cc/data.h>
#include <dhcp/duid.h>
#include <dhcpsrv/cfg_duid.h>
#include <dhcpsrv/cfgmgr.h>
#include <dhcpsrv/dhcpsrv_log.h>
#include <dhcpsrv/parsers/duid_config_parser.h>
#include <dhcpsrv/parsers/dhcp_parsers.h>
#include <exceptions/exceptions.h>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <limits>
#include <string>

using namespace isc::data;

namespace isc {
namespace dhcp {

void
DUIDConfigParser::parse(const CfgDUIDPtr& cfg, isc::data::ConstElementPtr duid_configuration) {
    if (!cfg) {
        isc_throw(DhcpConfigError, "Must provide valid pointer to cfg when parsing duid");
    }

    bool type_present = false;
    BOOST_FOREACH(ConfigPair element, duid_configuration->mapValue()) {
        try {
            if (element.first == "type") {
                type_present = true;
                setType(cfg, element.second->stringValue());
            } else if (element.first == "identifier") {
                setIdentifier(cfg, element.second->stringValue());
            } else if (element.first == "htype") {
                setHType(cfg, element.second->intValue());
            } else if (element.first == "time") {
                setTime(cfg, element.second->intValue());
            } else if (element.first == "enterprise-id") {
                setEnterpriseId(cfg, element.second->intValue());
            } else if (element.first == "persist") {
                setPersist(cfg, element.second->boolValue());
            } else {
                isc_throw(DhcpConfigError, "unsupported configuration "
                          "parameter '" << element.first << "'");
            }
        } catch (const std::exception& ex) {
            // Append position.
            isc_throw(DhcpConfigError, ex.what() << " ("
                      << element.second->getPosition() << ")");
        }
    }

    // "type" is mandatory
    if (!type_present) {
        isc_throw(DhcpConfigError, "mandatory parameter \"type\" not specified"
                  " for the DUID configuration ("
                  << duid_configuration->getPosition() << ")");
    }

    LOG_WARN(dhcpsrv_logger, DHCPSRV_CFGMGR_CONFIGURE_SERVERID);
}

void
DUIDConfigParser::setType(const CfgDUIDPtr& cfg, const std::string& duid_type) const {
    // Map DUID type represented as text into numeric value.
    DUID::DUIDType numeric_type = DUID::DUID_UNKNOWN;
    if (duid_type == "LLT") {
        numeric_type = DUID::DUID_LLT;
    } else if (duid_type == "EN") {
        numeric_type = DUID::DUID_EN;
    } else if (duid_type == "LL") {
        numeric_type = DUID::DUID_LL;
    } else {
        isc_throw(DhcpConfigError, "unsupported DUID type '"
                  << duid_type << "'. Expected: LLT, EN or LL");
    }

    cfg->setType(static_cast<DUID::DUIDType>(numeric_type));
}

void
DUIDConfigParser::setIdentifier(const CfgDUIDPtr& cfg, const std::string& identifier) const {
    cfg->setIdentifier(identifier);
}

void
DUIDConfigParser::setHType(const CfgDUIDPtr& cfg, const int64_t htype) const {
    checkRange<uint16_t>("htype", htype);
    cfg->setHType(static_cast<uint16_t>(htype));
}

void
DUIDConfigParser::setTime(const CfgDUIDPtr& cfg, const int64_t new_time) const {
    checkRange<uint32_t>("time", new_time);
    cfg->setTime(static_cast<uint32_t>(new_time));
}

void
DUIDConfigParser::setEnterpriseId(const CfgDUIDPtr& cfg, const int64_t enterprise_id) const {
    checkRange<uint32_t>("enterprise-id", enterprise_id);
    cfg->setEnterpriseId(static_cast<uint32_t>(enterprise_id));
}

void
DUIDConfigParser::setPersist(const CfgDUIDPtr& cfg, const bool persist) {
    cfg->setPersist(persist);
}

template<typename NumericType>
void
DUIDConfigParser::checkRange(const std::string& parameter_name,
                             const int64_t parameter_value) const {
    if ((parameter_value < 0) ||
        (parameter_value > std::numeric_limits<NumericType>::max())) {
        isc_throw(DhcpConfigError, "out of range value '" << parameter_value
                  << "' specified for parameter '" << parameter_name
                  << "'; expected value in range of [0.."
                  << std::numeric_limits<NumericType>::max() << "]");
    }
}


} // end of namespace isc::dhcp
} // end of namespace isc
