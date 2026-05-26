#ifndef IRODS_API_NUMBER_MAP_H
#define IRODS_API_NUMBER_MAP_H

#include <string>
#include <unordered_map>

#define API_NUMBER(NAME, VALUE) {VALUE, #NAME},

namespace irods
{
    /// \cond IRODS_DOXYGEN_INTERNAL
    const std::unordered_map<int, std::string> api_number_names{
        #include "irods/apiNumberData.h"
    };
    /// \endcond
} // namespace irods

#undef API_NUMBER

#endif  // IRODS_API_NUMBER_MAP_H
