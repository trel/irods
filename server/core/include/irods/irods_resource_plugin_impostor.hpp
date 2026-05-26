#ifndef IRODS_RESOURCE_PLUGIN_IMPOSTOR_HPP
#define IRODS_RESOURCE_PLUGIN_IMPOSTOR_HPP

#include "irods/irods_resource_plugin.hpp"

/// \file

namespace irods {

    /// Implements a resource plugin impostor used during plugin resolution.
    class impostor_resource : public irods::resource {
        public:
            /// Constructs an impostor resource with the given instance name and context.
            impostor_resource(
                const std::string& _inst_name,
                const std::string& _context );

            /// Reports whether post-disconnect maintenance is needed.
            error need_post_disconnect_maintenance_operation( bool& _b );

            /// Returns the post-disconnect maintenance operation to run.
            error post_disconnect_maintenance_operation( pdmo_type& _op );

            /// Reports an error for the provided plugin context.
            static error report_error(
                plugin_context& );

    }; // class impostor_resource

}; // namespace irods


#endif // IRODS_RESOURCE_PLUGIN_IMPOSTOR_HPP


