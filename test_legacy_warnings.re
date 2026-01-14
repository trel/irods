/* Test rules using legacy constructs to trigger deprecation warnings */

/* Test 1: forExec legacy loop */
test_forExec {
    forExec(x = 1; x < 5; x = x + 1) {
        writeLine("stdout", "x = " ++ str(x));
    }
}

/* Test 2: whileExec legacy loop */
test_whileExec {
    x = 0;
    whileExec(x < 5) {
        writeLine("stdout", "x = " ++ str(x));
        x = x + 1;
    }
}

/* Test 3: assign legacy function */
test_assign {
    assign(*x, 42);
    writeLine("stdout", "x = " ++ str(*x));
}

/* Test 4: delayExec legacy delay */
test_delayExec {
    delayExec("<PLUSET>1m</PLUSET>", "writeLine('stdout', 'delayed action')", "");
}

/* Test 5: remoteExec legacy remote execution */
test_remoteExec {
    remote("host.example.com", "rods") {
        writeLine("stdout", "remote action");
    }
}

/* Test 6: ifExec legacy conditional (combined with other operations) */
test_ifExec_legacy {
    x = 10;
    if (x > 5) then {
        writeLine("stdout", "x is greater than 5");
    } else {
        writeLine("stdout", "x is less than or equal to 5");
    }
}

/* Test 7: forEachExec legacy iteration */
test_forEachExec {
    forEachExec(*item in list(1, 2, 3, 4, 5)) {
        writeLine("stdout", "item = " ++ str(*item));
    }
}

/* Test 8: Modern for loop (no warning) */
test_for_modern {
    for (x = 1; x < 5; x = x + 1) {
        writeLine("stdout", "x = " ++ str(x));
    }
}

/* Test 9: Modern while loop (no warning) */
test_while_modern {
    x = 0;
    while (x < 5) {
        writeLine("stdout", "x = " ++ str(x));
        x = x + 1;
    }
}

/* Test 10: Modern foreach loop (no warning) */
test_foreach_modern {
    foreach (*item in list(1, 2, 3, 4, 5)) {
        writeLine("stdout", "item = " ++ str(*item));
    }
}

/* Test 11: Modern delay construct (no warning) */
test_delay_modern {
    delay("<PLUSET>1m</PLUSET>") {
        writeLine("stdout", "delayed action");
    }
}

/* Test 12: Modern remote construct (no warning) */
test_remote_modern {
    remote("host.example.com", "rods") {
        writeLine("stdout", "remote action");
    }
}
