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

        /// Constructs a parser and associates it with a configuration file path.
        configuration_parser(const std::string&);

        /// Replaces this parser with a copy of another parser.
        configuration_parser& operator=(const configuration_parser&);

        /// Removes all stored configuration values.
        void clear();

        /// Loads configuration data from a file.
        ///
        /// eturn An error object describing the outcome.
        error load(const std::string&);

        /// Writes the configuration to a file.
        ///
        /// eturn An error object describing the outcome.
        error write(const std::string&);

        /// Writes the configuration to the previously associated file.
        ///
        /// eturn An error object describing the outcome.
        error write();

        /// Returns whether a top-level key exists.
        ///
        /// eturn `true` if the key exists, otherwise `false`.
        bool has_entry(const std::string_view _key) const;

        /// Sets the value for a top-level key.
        ///
        /// 	param T The value type to store.
        /// eturn A reference to the stored value.
        template <typename T>
        T& set(const std::string& _key, const T& _val)
        {
            root_[_key] = boost::any(_val);
            return boost::any_cast<T&>(root_[_key]);
        } // set

        /// Sets the value identified by a nested key path.
        ///
        /// 	param T The value type to store.
        /// eturn A reference to the stored value.
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
        /// 	param T The requested value type.
        /// eturn A reference to the stored value.
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
        /// 	param T The requested value type.
        /// eturn A reference to the stored value.
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
        /// 	param T The expected value type.
        /// eturn The removed value.
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
        void remove(const std::string& _key);

        /// Returns the underlying top-level configuration map.
        ///
        /// eturn The root configuration map.
        std::unordered_map<std::string, boost::any>& map()
        {
            return root_;
        }

    private:
        /// Loads configuration data from a JSON file.
        error load_json_object(const std::string& _filename);
        /// Loads configuration data from a JSON object.
        error load_json_object(const nlohmann::json& _json);

        /// Converts a JSON value into its internal representation.
        boost::any convert_json(const nlohmann::json& _json);

        /// Replaces the current configuration map using copy-and-swap.
        error copy_and_swap(const std::unordered_map<std::string, boost::any>& _object_to_swap_in);

        std::string file_name_; ///< Full path to the associated configuration file.
        std::unordered_map<std::string, boost::any> root_; ///< Root configuration object.
    }; // class configuration_parser

    /// Converts a configuration key into its environment-variable form.
    std::string to_env( const std::string& );
} // namespace irods

#endif // IRODS_CONFIGURATION_PARSER_HPP
