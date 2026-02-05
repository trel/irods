"""
Integration tests for run_as_admin construct
Tests privilege escalation enforcement
"""

from __future__ import print_function
import unittest
import os
from textwrap import dedent

from .. import lib
from .. import test
from ..configuration import IrodsConfig
from ..controller import IrodsController
from . import resource_suite
from . import session


class Test_RunAsAdmin(resource_suite.ResourceBase, unittest.TestCase):
    """Test run_as_admin construct for privilege escalation."""

    def setUp(self):
        super(Test_RunAsAdmin, self).setUp()
        self.admin_session = session.make_session_for_existing_admin()
        
        # Create regular user
        self.admin_session.assert_icommand(['iadmin', 'mkuser', 'testuser_admin', 'rodsuser'])
        self.admin_session.assert_icommand(['iadmin', 'moduser', 'testuser_admin', 'password', 'testpass'])

    def tearDown(self):
        # Cleanup
        self.admin_session.run_icommand(['iadmin', 'rmuser', 'testuser_admin'])
        self.admin_session.__exit__()
        super(Test_RunAsAdmin, self).tearDown()



    def test_run_as_admin_allowed_for_rodsadmin(self):
        """Admin user should be able to execute run_as_admin."""
        # Try run_as_admin as admin user
        rule = 'run_as_admin { msiGetLogPathDelim(*x); }'
        
        # Should succeed without error
        self.admin_session.assert_icommand(
            ['irule', rule, 'null', 'ruleExecOut']
        )

    def test_run_as_admin_denied_for_regular_user(self):
        """Regular user should NOT be able to execute run_as_admin."""
        # Create session for regular user
        user_session = session.make_session_for_existing_user(
            'testuser_admin',
            'testpass',
            self.admin_session.environment_file_contents['irods_host'],
            self.admin_session.zone_name
        )
        
        try:
            rule = 'run_as_admin { msiGetLogPathDelim(*x); }'
            
            # Should fail with SYS_NO_API_PRIV error
            user_session.assert_icommand_fail(
                ['irule', rule, 'null', 'ruleExecOut'],
                'STDERR',
                'SYS_NO_API_PRIV'
            )
        finally:
            user_session.__exit__()

    def test_run_as_admin_via_pep_succeeds_for_regular_user(self):
        """Regular user should succeed in executing run_as_admin when invoked via PEP."""
        config = IrodsConfig()
        core_re_path = os.path.join(config.core_re_directory, 'core.re')

        # Create session for regular user
        user_session = session.make_session_for_existing_user(
            'testuser_admin',
            'testpass',
            self.admin_session.environment_file_contents['irods_host'],
            self.admin_session.zone_name
        )

        try:
            with lib.file_backed_up(core_re_path):
                try:
                    # Add a PEP that calls run_as_admin to verify a regular user can execute it via PEP
                    # This demonstrates the PEP context allows operations that irule context would deny
                    with open(core_re_path, 'a') as core_re:
                        core_re.write('pep_api_coll_create_pre(*a, *b, *c) { run_as_admin { msiGetLogPathDelim(*x); } }\\n')
                    
                    IrodsController(config).reload_configuration()

                    # Regular user should be able to trigger the PEP which calls run_as_admin
                    # Create a subcollection within an existing collection
                    user_session.assert_icommand(['imkdir', "pep_run_as_admin_test"])

                finally:
                    IrodsController(config).reload_configuration()
        finally:
            user_session.__exit__()


if __name__ == '__main__':
    unittest.main()
