#ifndef IRODS_CONFIGURATION_PARSER_HPP
#define IRODS_CONFIGURATION_PARSER_HPP

/// \file

#include "irods/irods_exception.hpp"
#include "irods/irods_error.hpp"
#include "irods/rodsErrorTable.h"

#include <vector>
#include <string>
#include <string_view>

#include <unordered_map>
#include <boost/format.hpp>
#include <boost/any.hpp>
#include <boost/optional.hpp>

#include <nlohmann/json.hpp>

namespace irods
{
    /// Stores and retrieves hierarchical configuration values.
    class configuration_parser
    {
    public:
        /// Sequence of keys describing a path through nested configuration objects.
        using key_path_t = std::vector<std::string>;

        /// Constructs an empty configuration parser.
        configuration_parser();

        /// Destroys the configuration parser.
        ~configuration_parser();

        /// Constructs a parser by copying another parser.
        configuration_parser(const configuration_parser& _other);

        /// Constructs a parser and associates it with the provided configuration file path.
        configuration_parser(const std::string&);

        /// Replaces this parser with a copy of another parser.
        configuration_parser& operator=(const configuration_parser&);

        /// Removes all stored configuration values.
        void clear();

        /// Loads configuration data from the file identified by the argument.
        ///
        /// \return An error object describing the outcome.
        error load(const std::string&);

        /// Writes the configuration to the file identified by the argument.
        ///
        /// \return An error object describing the outcome.
        error write(const std::string&);

        /// Writes the configuration to the previously associated file.
        ///
        /// \return An error object describing the outcome.
        error write();

        /// Returns whether a top-level key exists.
        ///
        /// \param[in] _key The key to inspect.
        /// \return `true` if the key exists, otherwise `false`.
        bool has_entry(const std::string_view _key) const;

        /// Sets the value for a top-level key.
        ///
        /// \tparam T The value type to store.
        /// \param[in] _key The key to update.
        /// \param[in] _val The value to store.
        /// \return A reference to the stored value.
        template <typename T>
        T& set(const std::string& _key, const T& _val)
        {
            root_[_key] = boost::any(_val);
            return boost::any_cast<T&>(root_[_key]);
        } // set

        /// Sets the value identified by a nested key path.
        ///
        /// \tparam T The value type to store.
        /// \param[in] _keys The nested key path to update.
        /// \param[in] _val The value to store.
        /// \return A reference to the stored value.
        template <typename T>
        T& set(const key_path_t& _keys, const T& _val)
        {
            if (_keys.empty()) {
                THROW(SYS_INVALID_INPUT_PARAM, "\"set\" requires at least one key");
            }

            boost::optional<boost::any&> cur_val;
            for (const auto& key : _keys) {
                if (!cur_val) {
                    cur_val.reset(root_[key]);
                    continue;
                }

                if (cur_val->empty()) {
                    *cur_val = std::unordered_map<std::string, boost::any>();
                }

                try {
                    cur_val.reset(boost::any_cast<std::unordered_map<std::string, boost::any>&>(*cur_val)[key]);
                }
                catch (const boost::bad_any_cast&) {
                    THROW(INVALID_ANY_CAST, "value was not a map");
                }
            }

            *cur_val = _val;

            return boost::any_cast<T&>(*cur_val);
        } // set with path

        /// Returns the value stored at a top-level key.
        ///
        /// \tparam T The requested value type.
        /// \param[in] _key The key to inspect.
        /// \return A reference to the stored value.
        template <typename T>
        T& get(const std::string& _key)
        {
            try {
                return boost::any_cast<T&>(root_.at(_key));
            }
            catch (const boost::bad_any_cast&) {
                THROW(INVALID_ANY_CAST, (boost::format("value at %s was incorrect type") % _key).str());
            }
            catch (const std::out_of_range&) {
                THROW (KEY_NOT_FOUND, (boost::format("key \"%s\" not found in map.") % _key).str());
            }
        } // get

        /// Returns the value stored at a nested key path.
        ///
        /// \tparam T The requested value type.
        /// \param[in] _keys The nested key path to inspect.
        /// \return A reference to the stored value.
        template <typename T>
        T& get(const key_path_t& _keys)
        {
            if (_keys.empty()) {
                THROW(SYS_INVALID_INPUT_PARAM, "\"get\" requires at least one key");
            }

            boost::optional<boost::any&> cur_val;
            for (const auto& key : _keys) {
                try {
                    if (!cur_val) {
                        cur_val.reset(root_.at(key));
                        continue;
                    }

                    try {
                        cur_val.reset(boost::any_cast<std::unordered_map<std::string, boost::any>&>(*cur_val).at(key));
                    }
                    catch (const boost::bad_any_cast&) {
                        THROW(INVALID_ANY_CAST, "value was not a map");
                    }
                }
                catch (const std::out_of_range&) {
                    THROW(KEY_NOT_FOUND, (boost::format("key \"%s\" not found in map.") % key).str());
                }
            }

            try {
                return boost::any_cast<T&>(*cur_val);
            }
            catch (const boost::bad_any_cast&) {
                THROW(INVALID_ANY_CAST, "value was incorrect type");
            }
        } // get with path

        /// Removes a top-level key and returns its value.
        ///
        /// \tparam T The expected value type.
        /// \param[in] _key The key to remove.
        /// \return The removed value.
        template <typename T>
        T remove(const std::string& _key)
        {
            auto find_it = root_.find(_key);
            if (find_it == root_.end()) {
                THROW(KEY_NOT_FOUND, (boost::format("key \"%s\" not found in map.") % _key).str());
            }
            T val = find_it->second;
            root_.erase(find_it);
            return val;
        }

        /// Removes a top-level key without returning its value.
        ///
        /// \param[in] _key The key to remove.
        void remove(const std::string& _key);

        /// Returns the underlying top-level configuration map.
        ///
        /// \return The root configuration map.
        std::unordered_map<std::string, boost::any>& map()
        {
            return root_;
        }

    private:
        /// Loads configuration data from a JSON file.
        ///
        /// \param[in] _filename Path to the JSON file.
        /// \return An error object describing the outcome.
        error load_json_object(const std::string& _filename);

        /// Loads configuration data from an in-memory JSON object.
        ///
        /// \param[in] _json JSON object to convert.
        /// \return An error object describing the outcome.
        error load_json_object(const nlohmann::json& _json);

        /// Converts a JSON value into its internal representation.
        ///
        /// \param[in] _json JSON value to convert.
        /// \return Converted configuration value.
        boost::any convert_json(const nlohmann::json& _json);

        /// Replaces the current configuration map using copy-and-swap.
        ///
        /// \param[in] _object_to_swap_in New root object to adopt.
        /// \return An error object describing the outcome.
        error copy_and_swap(const std::unordered_map<std::string, boost::any>& _object_to_swap_in);

        std::string file_name_; ///< Full path to the associated configuration file.
        std::unordered_map<std::string, boost::any> root_; ///< Root configuration object.
    }; // class configuration_parser

    /// Converts a configuration key into its environment-variable form.
    ///
    /// \return The environment-variable representation of the provided key.
    std::string to_env( const std::string& );
} // namespace irods

#endif // IRODS_CONFIGURATION_PARSER_HPP
