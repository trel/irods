#ifndef IRODS_RE_NAMESPACESHELPER_HPP
#define IRODS_RE_NAMESPACESHELPER_HPP

#include "irods/irods_server_properties.hpp"

#include "irods/rodsLog.h"

#include <vector>
#include <string>

class NamespacesHelper {
public:
    /// Returns the singleton namespace helper instance.
    static NamespacesHelper* Instance();
    /// Returns the configured rule engine namespaces.
    std::vector<std::string> getNamespaces();
protected:
private:
    NamespacesHelper(){};
    static NamespacesHelper* _instance;
    static std::vector<std::string> namespaces;
};

#endif
