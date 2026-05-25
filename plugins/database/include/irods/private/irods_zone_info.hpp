#ifndef _IRODS_ZONE_INFO_HPP_
#define _IRODS_ZONE_INFO_HPP_

#include "irods/irods_error.hpp"

#include "irods/icatStructs.hpp"

#include <string>

namespace irods {

    /// @brief Provides access to zone metadata cached by the database plugin.
    class zone_info {
        public:
            /// @brief Destroys a zone_info instance.
            virtual ~zone_info();

            /// @brief Returns the singleton zone_info instance.
            static zone_info* get_instance( void );

            /// @brief Returns the local zone name.
            error get_local_zone( icatSessionStruct _icss, int _logSQL, std::string& _rtn_local_zone );

        private:
            /// @brief Constructs a zone_info instance.
            zone_info( void );

            /// @brief Holds the singleton zone_info instance.
            static zone_info* the_instance_;

            /// @brief Caches the local zone name.
            std::string local_zone_;
    };
}; // namespace irods

#endif // _IRODS_ZONE_INFO_HPP_
