@backwardCompatible false

# Test 1: Simple query with 3 columns
testQueryColumns1() :-
  select COLL_NAME, DATA_NAME, DATA_SIZE

# Test 2: Query with single column
testQueryColumns2() :-
  select COLL_NAME

# Test 3: Query with WHERE clause (column extraction should stop before WHERE)
testQueryColumns3() :-
  select COLL_NAME, DATA_NAME, DATA_SIZE where COLL_NAME like '/home/%'

# Test 4: Complex WHERE clause with multiple conditions
testQueryColumns4() :-
  select USER_NAME, USER_ID where USER_NAME = 'admin' and USER_ID > 1000

# Test 5: Query with many columns
testQueryColumns5() :-
  select COLL_NAME, DATA_NAME, DATA_SIZE, RESC_NAME, MODIFY_TIME
