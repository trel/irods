#ifndef __IRODS_PHYSICAL_OBJECT_HPP__
#define  __IRODS_PHYSICAL_OBJECT_HPP__

// =-=-=-=-=-=-=-
// stl includes
#include <string>
#include "irods/objInfo.h"
#include "irods/rodsType.h"

namespace irods {

    /// Represents the physical-replica attributes of a data object.
    class physical_object {
        public:
            // =-=-=-=-=-=-=-
            // constructors
            /// \brief Constructs an empty physical object.
            physical_object();

            /// \brief Copy-constructs a physical object from another instance.
            physical_object( const physical_object& );

            /// \brief Constructs a physical object from catalog replica information.
            /// \param[in] _rhs Replica information used to populate the object.
            explicit physical_object(const DataObjInfo& _rhs);

            // =-=-=-=-=-=-=-
            // destructor
            /// \brief Destroys the physical object.
            ~physical_object();

            // =-=-=-=-=-=-=-
            // operators
            /// \brief Copies the state of another physical object into this instance.
            /// \return Reference to this object.
            physical_object& operator=( const physical_object& );

            // =-=-=-=-=-=-=-
            // accessors
            /// \brief Returns the replica status.
            /// \return The replica status value.
            inline int         replica_status()  const {
                return replica_status_;
            }

            /// \brief Returns the replica number.
            /// \return The replica number.
            inline int         repl_num()        const {
                return repl_num_;
            }

            /// \brief Returns the map id.
            /// \return The current map id.
            inline long        map_id()          const {
                return map_id_;
            }

            /// \brief Returns the replica size in bytes.
            /// \return The current size.
            inline long        size()            const {
                return size_;
            }

            /// \brief Returns the data object id.
            /// \return The current data object id.
            inline long        id()              const {
                return id_;
            }

            /// \brief Returns the collection id.
            /// \return The current collection id.
            inline long        coll_id()         const {
                return coll_id_;
            }

            /// \brief Returns the data object name.
            /// \return The current data object name.
            inline std::string name()            const {
                return name_;
            }

            /// \brief Returns the version string.
            /// \return The current version string.
            inline std::string version()         const {
                return version_;
            }

            /// \brief Returns the type name.
            /// \return The current type name.
            inline std::string type_name()       const {
                return type_name_;
            }

            /// \brief Returns the resource name.
            /// \return The current resource name.
            inline std::string resc_name()       const {
                return resc_name_;
            }

            /// \brief Returns the physical path.
            /// \return The current physical path.
            inline std::string path()            const {
                return path_;
            }

            /// \brief Returns the owner user name.
            /// \return The current owner name.
            inline std::string owner_name()      const {
                return owner_name_;
            }

            /// \brief Returns the owner zone.
            /// \return The current owner zone.
            inline std::string owner_zone()      const {
                return owner_zone_;
            }

            /// \brief Returns the status string.
            /// \return The current status.
            inline std::string status()          const {
                return status_;
            }

            /// \brief Returns the checksum.
            /// \return The current checksum string.
            inline std::string checksum()        const {
                return checksum_;
            }

            /// \brief Returns the expiry timestamp.
            /// \return The current expiry timestamp.
            inline std::string expiry_ts()       const {
                return expiry_ts_;
            }

            /// \brief Returns the mode string.
            /// \return The current mode string.
            inline std::string mode()            const {
                return mode_;
            }

            /// \brief Returns the replica comment.
            /// \return The current replica comment.
            inline std::string r_comment()       const {
                return r_comment_;
            }

            /// \brief Returns the create timestamp.
            /// \return The current create timestamp.
            inline std::string create_ts()       const {
                return create_ts_;
            }

            /// \brief Returns the modify timestamp.
            /// \return The current modify timestamp.
            inline std::string modify_ts()       const {
                return modify_ts_;
            }

            /// \brief Returns the resource hierarchy string.
            /// \return The current resource hierarchy.
            inline std::string resc_hier()       const {
                return resc_hier_;
            }

            /// \brief Returns the leaf resource id.
            /// \return The current resource id.
            inline rodsLong_t resc_id()          const {
                return resc_id_;
            }

            /// \brief Returns the replica vote.
            /// \return The current vote value.
            inline float vote() const
            {
                return vote_;
            } // vote

            // =-=-=-=-=-=-=-
            // mutators
            /// \brief Sets the replica status.
            /// \param[in] _v New replica status value.
            inline void replica_status( int _v )                 {
                replica_status_  = _v;
            }

            /// \brief Sets the replica number.
            /// \param[in] _v New replica number.
            inline void repl_num( int _v )                       {
                repl_num_        = _v;
            }

            /// \brief Sets the map id.
            /// \param[in] _v New map id.
            inline void map_id( int _v )                         {
                map_id_          = _v;
            }

            /// \brief Sets the replica size in bytes.
            /// \param[in] _v New size.
            inline void size( int _v )                           {
                size_            = _v;
            }

            /// \brief Sets the data object id.
            /// \param[in] _v New data object id.
            inline void id( int _v )                             {
                id_              = _v;
            }

            /// \brief Sets the collection id.
            /// \param[in] _v New collection id.
            inline void coll_id( int _v )                        {
                coll_id_         = _v;
            }

            /// \brief Sets the data object name.
            /// \param[in] _v New data object name.
            inline void name( const std::string& _v )            {
                name_            = _v;
            }

            /// \brief Sets the version string.
            /// \param[in] _v New version string.
            inline void version( const std::string& _v )        {
                version_         = _v;
            }

            /// \brief Sets the type name.
            /// \param[in] _v New type name.
            inline void type_name( const std::string& _v )       {
                type_name_       = _v;
            }

            /// \brief Sets the resource name.
            /// \param[in] _v New resource name.
            inline void resc_name( const std::string& _v )       {
                resc_name_       = _v;
            }

            /// \brief Sets the physical path.
            /// \param[in] _v New physical path.
            inline void path( const std::string& _v )            {
                path_            = _v;
            }

            /// \brief Sets the owner user name.
            /// \param[in] _v New owner name.
            inline void owner_name( const std::string& _v )      {
                owner_name_      = _v;
            }

            /// \brief Sets the owner zone.
            /// \param[in] _v New owner zone.
            inline void owner_zone( const std::string& _v )      {
                owner_zone_      = _v;
            }

            /// \brief Sets the status string.
            /// \param[in] _v New status string.
            inline void status( const std::string& _v )          {
                status_          = _v;
            }

            /// \brief Sets the checksum.
            /// \param[in] _v New checksum string.
            inline void checksum( const std::string& _v )        {
                checksum_        = _v;
            }

            /// \brief Sets the expiry timestamp.
            /// \param[in] _v New expiry timestamp.
            inline void expiry_ts( const std::string& _v )       {
                expiry_ts_       = _v;
            }

            /// \brief Sets the mode string.
            /// \param[in] _v New mode string.
            inline void mode( const std::string& _v )            {
                mode_            = _v;
            }

            /// \brief Sets the replica comment.
            /// \param[in] _v New replica comment.
            inline void r_comment( const std::string& _v )       {
                r_comment_       = _v;
            }

            /// \brief Sets the create timestamp.
            /// \param[in] _v New create timestamp.
            inline void create_ts( const std::string& _v )       {
                create_ts_       = _v;
            }

            /// \brief Sets the modify timestamp.
            /// \param[in] _v New modify timestamp.
            inline void modify_ts( const std::string& _v )       {
                modify_ts_       = _v;
            }

            /// \brief Sets the resource hierarchy string.
            /// \param[in] _v New resource hierarchy.
            inline void resc_hier( const std::string& _v )       {
                resc_hier_       = _v;
            }

            /// \brief Sets the leaf resource id.
            /// \param[in] _id New resource id.
            inline void resc_id( rodsLong_t _id )                  {
                resc_id_ = _id;
            }

            /// \brief Sets the replica vote.
            /// \param[in] vote New vote value.
            inline void vote(const float vote)
            {
                vote_ = vote;
            } // vote

        private:
            int         replica_status_;
            int         repl_num_;
            long        map_id_;
            long        size_;
            long        id_;
            long        coll_id_;
            std::string name_;
            std::string version_;
            std::string type_name_;
            std::string resc_name_;
            std::string path_;
            std::string owner_name_;
            std::string owner_zone_;
            std::string status_;
            std::string checksum_;
            std::string expiry_ts_;
            std::string mode_;
            std::string r_comment_;
            std::string create_ts_;
            std::string modify_ts_;
            std::string resc_hier_;
            rodsLong_t  resc_id_;
            float       vote_;

    }; // physical_object

}; // namespace irods

#endif // __IRODS_PHYSICAL_OBJECT_HPP__

