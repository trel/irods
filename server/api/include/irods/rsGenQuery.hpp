#ifndef RS_GEN_QUERY_HPP
#define RS_GEN_QUERY_HPP

#include "irods/rodsConnect.h"
#include "irods/rodsGenQuery.h"
#include <string>

/// \file

namespace irods {
    /// Owns and initializes a \p genQueryInp_t instance.
    class GenQueryInpWrapper {
        /// Holds the wrapped query input structure.
        genQueryInp_t genquery_inp_;
    public:
        /// Constructs an empty query input wrapper.
        explicit GenQueryInpWrapper(void);

        /// Releases resources owned by the wrapped query input.
        ~GenQueryInpWrapper(void);

        /// Returns the wrapped query input structure.
        genQueryInp_t& get(void);
    };

    /// Owns and cleans up a \p genQueryOut_t pointer.
    class GenQueryOutPtrWrapper {
        /// Holds the wrapped query output pointer.
        genQueryOut_t* genquery_out_ptr_;
    public:
        /// Constructs an empty query output pointer wrapper.
        explicit GenQueryOutPtrWrapper(void);

        /// Releases resources owned by the wrapped query output pointer.
        ~GenQueryOutPtrWrapper(void);

        /// Returns the wrapped query output pointer.
        genQueryOut_t*& get(void);
    };
}

/// Returns a diagnostic string representation of a general query input.
std::string genquery_inp_to_diagnostic_string(const genQueryInp_t *q);

/// Returns an iquest string representation of a general query input.
std::string genquery_inp_to_iquest_string(const genQueryInp_t *q);

/// Executes a general query request.
int rsGenQuery( rsComm_t *rsComm, genQueryInp_t *genQueryInp, genQueryOut_t **genQueryOut );

/// Performs the server-side implementation of a general query request.
int _rsGenQuery( rsComm_t *rsComm, genQueryInp_t *genQueryInp, genQueryOut_t **genQueryOut );

#endif
