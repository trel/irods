#ifndef _IRODS_OBJECT_OPER_HPP_
#define _IRODS_OBJECT_OPER_HPP_

#include "irods/irods_error.hpp"
#include "irods/irods_file_object.hpp"

namespace irods {

    /**
     * @brief Class that holds a resource operation that needs to be replicated and the object on which it operated.
     */
    class object_oper {
        public:
            /// @brief Constructs an empty replicated-operation record.
            object_oper( void ) {}

            /// @brief Constructs a replicated-operation record from an object and operation name.
            object_oper( const file_object& _file_object, const std::string& _operation );

            /// @brief Destroys the replicated-operation record.
            virtual ~object_oper( void );

            /// @brief Returns the file object associated with the operation.
            const file_object& object( void ) const {
                return file_object_;
            }

            /// @brief Returns the operation name.
            const std::string& operation( void ) const {
                return operation_;
            }

            /// @brief Returns the file object associated with the operation.
            file_object& object( void ) {
                return file_object_;
            }

            /// @brief Returns the operation name.
            std::string& operation( void ) {
                return operation_;
            }

        private:
            /// @brief File object describing the replicated operation target.
            file_object file_object_;

            /// @brief Name of the operation to replicate.
            std::string operation_;
    };
}; // namespace irods

#endif // _IRODS_OBJECT_OPER_HPP_
