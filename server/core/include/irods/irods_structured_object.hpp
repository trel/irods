#ifndef __IRODS_STRUCTURED_OBJECT_HPP__
#define __IRODS_STRUCTURED_OBJECT_HPP__

// =-=-=-=-=-=-=-
// stl includes
#include <string>

// =-=-=-=-=-=-=-
#include "irods/irods_file_object.hpp"
#include "irods/irods_log.hpp"
#include "irods/subStructFileRead.h"

    namespace irods {

    /// @brief File object describing an entry within a structured file.
    class structured_object : public file_object {
        public:
            // =-=-=-=-=-=-=-
            // Constructors
            /// \brief Constructs an empty structured object.
            structured_object();

            /// \brief Copy-constructs a structured object from another instance.
            structured_object( const structured_object& );

            /// \brief Constructs a structured object from subfile input describing a structured-file member.
            structured_object( subFile_t& );

            /// \brief Constructs a structured object from structured-file file-descriptor input.
            structured_object( subStructFileFdOprInp_t& );

            // =-=-=-=-=-=-=-
            // Destructor
            /// \brief Destroys the structured object.
            virtual ~structured_object();

            // =-=-=-=-=-=-=-
            // Operators
            /// \brief Copies the state of another structured object into this instance.
            /// \return Reference to this object.
            virtual structured_object& operator=( const structured_object& );

            // =-=-=-=-=-=-=-
            // plugin resolution operation
            /// \brief Resolves a plugin implementing the requested interface.
            /// \return An error describing the result of plugin resolution.
            virtual error resolve(
                const std::string&, // plugin interface name
                plugin_ptr& );      // resolved plugin instance

            // =-=-=-=-=-=-=-
            // accessor for rule engine variables
            /// \brief Populates the rule-engine variable set for this object.
            /// \return An error describing the result of the population operation.
            virtual error get_re_vars( rule_engine_vars_t& );

            // =-=-=-=-=-=-=-
            // Accessors
            /// \brief Returns the host address for the structured-file operation.
            /// \return The current host address.
            rodsHostAddr_t addr()          const {
                return addr_;
            }

            /// \brief Returns the path of the member inside the structured file.
            /// \return The current subfile path.
            std::string sub_file_path() const {
                return sub_file_path_;
            }

            /// \brief Returns the byte offset for the structured-file operation.
            /// \return The current offset.
            size_t offset()        const {
                return offset_;
            }

            /// \brief Returns the special collection descriptor.
            /// \return The current special collection pointer.
            specColl_t* spec_coll()     const {
                return spec_coll_;
            }

            /// \brief Returns the structured-file data type.
            /// \return The current data type string.
            std::string data_type()     const {
                return data_type_;
            }

            /// \brief Returns the structured-file operation type.
            /// \return The current operation type.
            int opr_type()      const {
                return opr_type_;
            }

            /// \brief Returns the special collection type.
            /// \return The current special collection type.
            structFileType_t spec_coll_type() const {
                return spec_coll_type_;
            }

            // =-=-=-=-=-=-=-
            // Mutators
            /// \brief Sets the host address for the structured-file operation.
            /// \param[in] _addr New host address.
            void addr( const rodsHostAddr_t& _addr ) {
                addr_      = _addr;
            }

            /// \brief Sets the path of the member inside the structured file.
            /// \param[in] _p New subfile path.
            void sub_file_path( const std::string& _p ) {
                sub_file_path_ = _p;
            }

            /// \brief Sets the special collection descriptor.
            /// \param[in] _coll New special collection pointer.
            void spec_coll( specColl_t*    _coll ) {
                spec_coll_ = _coll;
            }

            /// \brief Sets the structured-file data type.
            /// \param[in] _dt New data type string.
            void data_type( const std::string&    _dt ) {
                data_type_ = _dt;
            }

            /// \brief Sets the structured-file operation type.
            /// \param[in] _ot New operation type.
            void opr_type( int            _ot ) {
                opr_type_ = _ot;
            }

            /// \brief Sets the special collection type.
            /// \param[in] _t New special collection type.
            void spec_coll_type( const structFileType_t& _t ) {
                spec_coll_type_ = _t;
            }

        protected:
            // =-=-=-=-=-=-=-
            // Attributes
            // NOTE :: These are not guaranteed to be properly populated right now
            //      :: that will need be done later when these changes are pushed
            //      :: higher in the original design
            /// \brief Host address associated with the structured-file operation.
            rodsHostAddr_t addr_;

            /// \brief Path of the member inside the structured file.
            std::string    sub_file_path_;

            /// \brief Byte offset associated with the operation.
            rodsLong_t     offset_;

            /// \brief Special collection descriptor for the structured file.
            specColl_t*    spec_coll_;

            /// \brief Data type associated with the structured-file entry.
            std::string    data_type_;

            /// \brief Operation type associated with the structured-file request.
            int            opr_type_;

            /// \brief Structured-file type for the special collection.
            structFileType_t spec_coll_type_;

    }; // class structured_object

/// =-=-=-=-=-=-=-
/// @brief typedef for shared structured object pointer
    typedef boost::shared_ptr< structured_object > structured_object_ptr;

}; // namespace irods

#endif // __IRODS_STRUCTURED_OBJECT_HPP__
