#ifndef __IRODS_FILE_OBJECT_HPP__
#define __IRODS_FILE_OBJECT_HPP__

// =-=-=-=-=-=-=-
// irods includes
#include "irods/fileCreate.h"
#include "irods/rodsConnect.h"
#include "irods/rodsType.h"

// =-=-=-=-=-=-=-
#include "irods/irods_data_object.hpp"
#include "irods/irods_hierarchy_parser.hpp"
#include "irods/irods_physical_object.hpp"

// =-=-=-=-=-=-=-
// stl includes
#include <optional>
#include <string_view>
#include <tuple>
#include <vector>

#include <nlohmann/json.hpp>

namespace irods {

    class file_object : public data_object {
        public:
            // =-=-=-=-=-=-=-
            // Constructors
            /// \brief Constructs an empty file object.
            file_object();

            /// \brief Copy-constructs a file object from another instance.
            file_object( const file_object& );

            /// \brief Constructs a file object using a resource id.
            /// \param[in] _comm Server connection used by the file object.
            /// \param[in] _logical_name Logical path of the data object.
            /// \param[in] _filename Physical path of the replica.
            /// \param[in] _resc_id Leaf resource id for the replica.
            /// \param[in] _fd File descriptor associated with the object.
            /// \param[in] _mode Open mode for the object.
            /// \param[in] _flags Operation flags.
            file_object(
                rsComm_t* _comm,
                const std::string& _logical_name,
                const std::string& _filename,
                rodsLong_t _resc_id,
                int _fd,
                int _mode,
                int _flags );

            /// \brief Constructs a file object using a resource hierarchy.
            /// \param[in] _comm Server connection used by the file object.
            /// \param[in] _logical_name Logical path of the data object.
            /// \param[in] _filename Physical path of the replica.
            /// \param[in] _resc_hier Resource hierarchy for the replica.
            /// \param[in] _fd File descriptor associated with the object.
            /// \param[in] _mode Open mode for the object.
            /// \param[in] _flags Operation flags.
            file_object(
                rsComm_t* _comm,
                const std::string& _logical_name,
                const std::string& _filename,
                const std::string& _resc_hier,
                int _fd,
                int _mode,
                int _flags );

            /// \brief Constructs a file object from catalog replica information.
            /// \param[in] _comm Server connection used by the file object.
            /// \param[in] _dataObjInfo Replica metadata used to populate the object.
            file_object(
                rsComm_t* _comm,
                const dataObjInfo_t* _dataObjInfo );

            // =-=-=-=-=-=-=-
            // Destructor
            /// \brief Destroys the file object.
            virtual ~file_object();

            // =-=-=-=-=-=-=-
            // Operators
            /// \brief Copies the state of another file object into this instance.
            /// \return Reference to this object.
            virtual file_object& operator=( const file_object& );

            /// @brief Comparison operator
            /// @param[in] _rhs File object to compare against.
            /// @return True if the file objects compare equal; otherwise false.
            virtual bool operator==( const file_object& _rhs ) const;

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

            /// \brief Returns the replica matching a resource hierarchy.
            /// \param[in] _hierarchy Resource hierarchy to match.
            ///
            /// \returns reference to the replica in replicas_ with a resource hierarchy matching _hierarchy
            /// \retval std::nullopt if a replica with resource hierarchy _hierarchy is not found in replicas_
            ///
            /// \since 4.2.9
            auto get_replica(std::string_view _hierarchy) -> std::optional<std::reference_wrapper<physical_object>>;

            /// \brief Returns the replica matching a replica number.
            /// \param[in] _replica_number Replica number to match.
            ///
            /// \returns reference to the replica in replicas_ with a replica number matching _replica_number
            /// \retval std::nullopt if a replica with replica number _replica_number is not found in replicas_
            ///
            /// \since 4.2.9
            auto get_replica(const int _replica_number) -> std::optional<std::reference_wrapper<physical_object>>;

            // =-=-=-=-=-=-=-
            // Accessors
            /// \brief Returns the server connection used by this object.
            /// \return The associated server connection.
            virtual rsComm_t*                      comm()            const {
                return comm_;
            }

            /// \brief Returns the logical path of the data object.
            /// \return The current logical path.
            virtual std::string                    logical_path()    const {
                return logical_path_;
            }

            /// \brief Returns the data type string.
            /// \return The current data type.
            virtual std::string                    data_type()       const {
                return data_type_;
            }

            /// \brief Returns the active file descriptor.
            /// \return The current file descriptor.
            virtual int                            file_descriptor() const {
                return file_descriptor_;
            }

            /// \brief Returns the L1 descriptor index.
            /// \return The current L1 descriptor index.
            virtual int                            l1_desc_idx()     const {
                return l1_desc_idx_;
            }

            /// \brief Returns the replica size in bytes.
            /// \return The current size.
            virtual rodsLong_t                         size()            const {
                return size_;
            }

            /// \brief Returns the requested replica number.
            /// \return The requested replica number.
            virtual int                            repl_requested()  const {
                return repl_requested_;
            }
            // IF IT BLOWS UP, THIS IS WHY (ref)
            /// \brief Returns the immutable replica list.
            /// \return The list of replica descriptions.
            virtual const std::vector< physical_object >& replicas() const {
                return replicas_;
            }

            /// \brief Returns the pdmo hierarchy marker.
            /// \return The current pdmo hierarchy string.
            virtual const std::string&             in_pdmo()         const {
                return in_pdmo_;
            }

            /// \brief Returns the data object id.
            /// \return The current data id.
            virtual long                           data_id()         const {
                return data_id_;
            }

            /// \brief Returns the collection id.
            /// \return The current collection id.
            virtual long                           coll_id()         const {
                return coll_id_;
            }

            // =-=-=-=-=-=-=-
            // Mutators
            /// \brief Sets the logical path of the data object.
            /// \param[in] _s New logical path.
            virtual void logical_path( const std::string& _s )   {
                logical_path_    = _s;
            }

            /// \brief Sets the active file descriptor.
            /// \param[in] _fd New file descriptor.
            virtual void file_descriptor( int _fd )              {
                file_descriptor_ = _fd;
            }

            /// \brief Sets the server connection used by this object.
            /// \param[in] _c New server connection.
            virtual void comm( rsComm_t* _c )                   {
                comm_            = _c;
            }

            /// \brief Sets the replica size in bytes.
            /// \param[in] _v New size.
            virtual void size( rodsLong_t _v )                       {
                size_            = _v;
            }

            /// \brief Sets the requested replica number.
            /// \param[in] _v Replica number to request.
            virtual void repl_requested( int _v )                {
                repl_requested_  = _v;
            }

            /// \brief Sets the pdmo hierarchy marker.
            /// \param[in] _v New pdmo hierarchy string.
            virtual void in_pdmo( const std::string& _v )        {
                in_pdmo_         = _v;
            }

            /// \brief Replaces the replica list.
            /// \param[in] _v New list of replica descriptions.
            virtual void replicas( const std::vector< physical_object >& _v ) {
                replicas_ = _v;
            }

            /// \brief Returns the mutable replica list.
            /// \return The mutable list of replica descriptions.
            virtual std::vector<physical_object>& replicas() {
                return replicas_;
            }

            /// \brief Sets the data object id.
            /// \param[in] _data_id New data object id.
            virtual void data_id(const long _data_id) {
                data_id_ = _data_id;
            }

            /// \brief Sets the collection id.
            /// \param[in] _coll_id New collection id.
            virtual void coll_id(const long _coll_id) {
                coll_id_ = _coll_id;
            }

        protected:
            // =-=-=-=-=-=-=-
            // Attributes
            // NOTE :: These are not guaranteed to be properly populated right now
            //      :: that will need be done later when these changes are pushed
            //      :: higher in the original design
            rsComm_t*                      comm_;            // connection to irods session
            std::string                    logical_path_;    // full logical path from icat
            std::string                    data_type_;       // data type as described in objInfo.h:32
            int                            file_descriptor_; // file descriptor, if the file is in flight
            int                            l1_desc_idx_;     // index into irods L1 file descriptor table
            rodsLong_t                     size_;            // size of the file in bytes
            int                            repl_requested_;  // requested replica number
            std::string                    in_pdmo_;         // hierarchy indicating the current operations are
            // occurring from within a pdmo
            // call made from within the hierarchy
            std::vector< physical_object > replicas_;        // structures holding replica info initialized
            long                           data_id_;
            long                           coll_id_;
            // by factory fcn from
            // dataObjInfoHead

    }; // class file_object

/// =-=-=-=-=-=-=-
/// @brief typedef for managed file object ptr
    typedef boost::shared_ptr< file_object > file_object_ptr;

// =-=-=-=-=-=-=-
// factory function which will take a dataObjInfo pointer and create a file_object
    /// \brief Builds a file object from a data object input structure and replica information.
    /// \param[in] _comm Server connection used to populate the file object.
    /// \param[in] _data_obj_inp Data object input describing the request.
    /// \param[in,out] _file_obj File object instance to populate.
    /// \param[out] _data_obj_info Optional destination for the located replica information.
    /// \return An error describing the result of the factory operation.
    error file_object_factory(rsComm_t*        _comm,
                              dataObjInp_t*    _data_obj_inp,
                              file_object_ptr  _file_obj,
                              dataObjInfo_t**  _data_obj_info = nullptr);

    /// \brief factory function which takes a data id and creates a file_object
    ///
    /// \param[in] _comm Server connection used to populate the file object.
    /// \param[in] _data_id Data object id used to locate replica information.
    /// \return A populated file object for the requested data id.
    ///
    /// \since 4.2.9
    auto file_object_factory(RsComm& _comm, const rodsLong_t _data_id) -> irods::file_object_ptr;

    /// \brief Factory function which takes a JSON array and creates a file_object
    ///
    /// \parblock
    /// Functions very similarly to file_object_factory(RsComm&, const rodsLong_t) after the
    /// data_object_proxy_t has been generated.
    /// \endparblock
    ///
    /// \param[in] _comm Handle to server connection structure.
    /// \param[in] _logical_path Full logical path of the data object being described.
    /// \param[in] _replicas A JSON array of replicas conforming to irods::experimental::replica::to_json.
    /// \return A populated file object for the provided logical path and replica set.
    ///
    /// \since 4.2.11
    auto file_object_factory(RsComm& _comm,
                             const std::string_view _logical_path,
                             const std::vector<const nlohmann::json*>& _replicas) -> irods::file_object_ptr;

    /// \brief Reports whether a file object contains a replica for a hierarchy.
    /// \param[in] _obj File object to search.
    /// \param[in] _hierarchy Resource hierarchy to search for.
    ///
    /// \retval true if replica with given resource hierarchy is found in the list of replicas
    /// \retval false if replica with given resource hierarchy is not found in the list of replicas
    ///
    /// \since 4.2.9
    auto hierarchy_has_replica(
        const irods::file_object_ptr _obj,
        std::string_view _hierarchy) -> bool;
}; // namespace irods

#endif // __IRODS_FILE_OBJECT_HPP__
