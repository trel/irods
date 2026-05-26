#ifndef __IRODS_FIRST_CLASS_OBJECT_HPP__
#define __IRODS_FIRST_CLASS_OBJECT_HPP__

// =-=-=-=-=-=-=-
#include "irods/irods_log.hpp"

// =-=-=-=-=-=-=-
// irods includes
#include "irods/rcConnect.h"

// =-=-=-=-=-=-=-
// boost includs
#include <boost/shared_ptr.hpp>
#include <map>

namespace irods {

    class plugin_base;
    typedef boost::shared_ptr<plugin_base>    plugin_ptr;
    typedef std::map<std::string,std::string> rule_engine_vars_t;
    /// @brief Common interface for objects passed to plugins.
    class first_class_object {
        public:
            /// @brief Constructs an empty first-class object.
            first_class_object() {};

            /// @brief Destroys the object through the base interface.
            virtual ~first_class_object() {};

            /// @brief Resolves a plugin for the requested interface.
            /// @details Input is a plugin interface name. Output is a resolved plugin instance.
            /// @return Error status describing resolution success or failure.
            virtual error resolve(
                const std::string&, // plugin interface
                plugin_ptr& ) = 0;  // resolved plugin

            /// @brief Exposes rule-engine variables for this object.
            /// @details Output is a populated rule-engine variable map.
            /// @return Error status describing extraction success or failure.
            virtual error get_re_vars( rule_engine_vars_t& ) = 0;

    }; // class first_class_object

    /// =-=-=-=-=-=-=-
    /// @brief shared pointer to first_class_object
    typedef boost::shared_ptr< first_class_object > first_class_object_ptr;

}; // namespace irods

#endif // __IRODS_FIRST_CLASS_OBJECT_HPP__

