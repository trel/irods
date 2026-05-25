#ifndef __IRODS_DATA_OBJECT_HPP__
#define __IRODS_DATA_OBJECT_HPP__

/// \file

// =-=-=-=-=-=-=-
#include "irods/irods_first_class_object.hpp"

// =-=-=-=-=-=-=-
// irods includes
#include "irods/rcConnect.h"
#include "irods/rcMisc.h"


namespace irods {

/// =-=-=-=-=-=-=-
/// @brief typedef for managed data object pointer
    class data_object;
    /// Shared pointer to a data object.
    typedef boost::shared_ptr< data_object > data_object_ptr;

// =-=-=-=-=-=-=-
// base class for all object types
    /// Represents a first-class data object handled by the server.
    class data_object : public first_class_object {
        public:
            // =-=-=-=-=-=-=-
            // Constructors
            /// \brief Constructs an empty data object.
            data_object();

            /// \brief Constructs a data object from a physical path, resource id, mode, and flags.
            data_object(
                const std::string&,		// phy path
                rodsLong_t,             // resc id
                int,                	// mode
                int );					// flags

            /// \brief Constructs a data object from a physical path, resource id, mode, flags, and conditional input.
            data_object(
                const std::string&,		// phy path
                rodsLong_t,             // resc id
                int,                	// mode
                int,					// flags
                const keyValPair_t& );	// cond_input

            /// \brief Constructs a data object from a physical path, resource hierarchy, mode, and flags.
            data_object(
                const std::string&,		// phy path
                const std::string&,		// resc hier
                int,                	// mode
                int );					// flags

            /// \brief Constructs a data object from a physical path, resource hierarchy, mode, flags, and conditional input.
            data_object(
                const std::string&,		// phy path
                const std::string&,		// resc hier
                int,                	// mode
                int,					// flags
                const keyValPair_t& );	// cond_input


            /// \brief Copy-constructs a data object from another instance.
            data_object( const data_object& );

            // =-=-=-=-=-=-=-
            // Destructor
            /// \brief Destroys the data object.
            virtual ~data_object();

            // =-=-=-=-=-=-=-
            // Operators
            /// \brief Copies the state of another data object into this instance.
            /// \return Reference to this object.
            virtual data_object& operator=( const data_object& );

            // =-=-=-=-=-=-=-
            // plugin resolution operators
            /// \brief Resolves a plugin implementing the requested interface.
            /// \return An error describing the result of plugin resolution.
            virtual error resolve(
                const std::string&, // plugin interface name
                plugin_ptr& ) = 0;  // resolved plugin instance

            // =-=-=-=-=-=-=-
            // accessor for rule engine variables
            /// \brief Populates the rule-engine variable set for this object.
            /// \return An error describing the result of the population operation.
            virtual error get_re_vars( rule_engine_vars_t& );

            // =-=-=-=-=-=-=-
            // Accessors

            /// \brief Returns the physical path in the vault.
            /// \return The current physical path.
            virtual std::string physical_path()   const {
                return physical_path_;
            }

            /// \brief Returns the resource hierarchy for this object.
            /// \return The current resource hierarchy string.
            virtual std::string resc_hier()       const {
                return resc_hier_;
            }

            /// \brief Returns the object identifier.
            /// \return The current object id.
            virtual long        id()              const {
                return id_;
            }

            /// \brief Returns the open or operation mode.
            /// \return The current mode value.
            virtual int         mode()            const {
                return mode_;
            }

            /// \brief Returns the object flags.
            /// \return The current flags value.
            virtual int         flags()           const {
                return flags_;
            }

            /// \brief Returns the immutable conditional input set.
            /// \return The conditional input key-value pairs.
            virtual const keyValPair_t& cond_input()	const {
                return cond_input_;
            }

            /// \brief Returns the leaf resource id used for the hierarchy.
            /// \return The current resource id.
            virtual rodsLong_t resc_id() const {
                return resc_id_;
            }

            // =-=-=-=-=-=-=-
            // Mutators
            /// \brief Sets the physical path in the vault.
            /// \param[in] _path New physical path.
            virtual void physical_path( const std::string& _path ) {
                physical_path_   = _path;
            }

            /// \brief Sets the resource hierarchy string.
            /// \param[in] _hier New resource hierarchy.
            virtual void resc_hier( const std::string& _hier )     {
                resc_hier_       = _hier;
            }

            /// \brief Sets the object identifier.
            /// \param[in] _id New object id.
            virtual void id( long _id ) {
                id_ = _id;
            }

            /// \brief Sets the open or operation mode.
            /// \param[in] _m New mode value.
            virtual void mode( int _m )                         {
                mode_            = _m;
            }

            /// \brief Sets the object flags.
            /// \param[in] _f New flags value.
            virtual void flags( int _f )                         {
                flags_           = _f;
            }

            /// \brief Replaces the conditional input set.
            /// \param[in] _cond_input New conditional input key-value pairs.
            virtual void cond_input( const keyValPair_t& _cond_input ) {
                replKeyVal( &_cond_input, &cond_input_ );
            }

            /// \brief Returns mutable conditional input key-value pairs.
            /// \return The mutable conditional input set.
            virtual keyValPair_t& cond_input()	{
                return cond_input_;
            }

            /// \brief Sets the leaf resource id used for the hierarchy.
            /// \param[in] _id New resource id.
            virtual void resc_id( rodsLong_t _id ) {
                resc_id_ = _id;
            }

            /// \brief Adds or replaces a conditional input key-value pair.
            /// \param[in,out] _do Data object receiving the key-value pair.
            /// \param[in] _k Key to add or update.
            /// \param[in] _v Value to associate with the key.
            friend void add_key_val(
                data_object_ptr&   _do,
                const std::string& _k,
                const std::string& _v );

            /// \brief Removes a conditional input key-value pair.
            /// \param[in,out] _do Data object to update.
            /// \param[in] _k Key to remove.
            friend void remove_key_val(
                data_object_ptr&   _do,
                const std::string& _k );

        protected:
            // =-=-=-=-=-=-=-
            // Attributes
            // NOTE :: These are not guaranteed to be properly populated right now
            //      :: that will need be done later when these changes are pushed
            //      :: higher in the original design
            /// Full physical path in the vault.
            std::string  physical_path_; // full physical path in the vault
            /// Resource hierarchy describing the object's location.
            std::string  resc_hier_;     // where this lives in the resource hierarchy
            /// Data object identifier.
            long        id_;             // object id
            /// Mode used when operating on the object.
            int          mode_;	         // mode when opened or modified
            /// Operation flags applied to the object.
            int          flags_;         // flags for object operations
            /// Conditional input key-value pairs.
            keyValPair_t cond_input_;    // input key-value pairs
            /// Leaf resource identifier used to produce the hierarchy.
            rodsLong_t   resc_id_;       // leaf resource id used to generate hierarchy

    }; // class data_object

}; // namespace irods

#endif // __IRODS_DATA_OBJECT_HPP__
