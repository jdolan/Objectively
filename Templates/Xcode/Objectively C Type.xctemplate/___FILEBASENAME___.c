//
//  ___FILENAME___
//  ___PROJECTNAME___
//
//  Created by ___FULLUSERNAME___ on ___DATE___.
//___COPYRIGHT___
//

#include <assert.h>

#include <___PROJECTNAME___/___FILEBASENAMEASIDENTIFIER___.h>

#define _Class ____FILEBASENAMEASIDENTIFIER___

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {
	
	//..
	
	super(Object, self, dealloc);
}

#pragma mark - ___FILEBASENAMEASIDENTIFIER___

/**
 * @fn ___FILEBASENAMEASIDENTIFIER___ *___FILEBASENAMEASIDENTIFIER___::init(___FILEBASENAMEASIDENTIFIER___ *self)
 * @memberof ___FILEBASENAMEASIDENTIFIER___
 */
static ___FILEBASENAMEASIDENTIFIER___ *init(___FILEBASENAMEASIDENTIFIER___ *self) {
	
	self = (___FILEBASENAMEASIDENTIFIER___ *) super(___VARIABLE_superclass:identifier___, self, init);
	if (self) {
		
		//..
	}
	
	return self;
}

//..

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {
	
	((ObjectInterface *) clazz->def->interface)->dealloc = dealloc;
	
	((___FILEBASENAMEASIDENTIFIER___Interface *) clazz->def->interface)->init = init;
	
	//..
}

/**
 * @fn Class *___FILEBASENAMEASIDENTIFIER___::____FILEBASENAMEASIDENTIFIER___(void)
 * @memberof ___FILEBASENAMEASIDENTIFIER___
 */
Class *____FILEBASENAMEASIDENTIFIER___(void) {
	static Class clazz;
	static Once once;

	do_once(&once, {
		clazz.name = "___FILEBASENAMEASIDENTIFIER___";
		clazz.superclass = ____VARIABLE_superclass:identifier___();
		clazz.instanceSize = sizeof(___FILEBASENAMEASIDENTIFIER___);
		clazz.interfaceOffset = offsetof(___FILEBASENAMEASIDENTIFIER___, interface);
		clazz.interfaceSize = sizeof(___FILEBASENAMEASIDENTIFIER___Interface);
		clazz.initialize = initialize;
	});

	return &clazz;
}

#undef _Class
