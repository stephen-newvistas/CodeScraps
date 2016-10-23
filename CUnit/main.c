#include <stdio.h>
#include <stdlib.h>
#include <CUnit/CUnit.h>
#include <CUnit/CUnit_intl.h>
#include <CUnit/Util.h>
#include <CUnit/Console.h>

#define TRUE 	1
#define FALSE 	0

int init_suite_success( void ){ return 0; }
int init_suite_failure( void ){ return -1; }
int clean_suite_success( void ){ return 0; }
int clean_suite_failure( void ){ return -1; }

void test_success1( void ){
	fries( 1 );
//	CU_ASSERT( TRUE );
}

void test_failure1( void ){
//	CU_ASSERT( FALSE );
	fries( 0 );
}

void fries( int _value ){
	if( _value ){
		puts( "p" );
		CU_PASS( "Yep, we passed." );
	}
	else{
		puts( "f" );
		CU_FAIL( "Nope, we failed." );
	}
}



int main(){

	//	create object
	CU_pSuite pSuite = NULL;

	//	initialize the registry
	if( CUE_SUCCESS != CU_initialize_registry( ) ){
		CU_cleanup_registry();
		return CU_get_error();
	}

	//	add a suite to the registry
	pSuite = CU_add_suite( "Suite Success" , init_suite_success , clean_suite_success );
	if( pSuite == NULL ){
		CU_cleanup_registry();
		return CU_get_error();
	}

	//	add tests to the suite
	if( CU_add_test( pSuite , "Success Test 1" , test_success1 ) == NULL ){
		CU_cleanup_registry();
		return CU_get_error();
	}

	//	add a suite to the registry
	pSuite = CU_add_suite( "Suite Failure" , init_suite_failure , clean_suite_failure );
	if( pSuite == NULL ){
		CU_cleanup_registry();
		return CU_get_error();
	}

	//	add tests to the suite
	if( CU_add_test( pSuite , "Failure Test 1" , test_failure1 ) == NULL ){
		CU_cleanup_registry();
		return CU_get_error();
	}

	//	run tests using the console
	CU_console_run_tests();
	CU_cleanup_registry();

}
