//
//  ___FILENAME___
//  ___PROJECTNAME___
//
//  Created by ___FULLUSERNAME___ on ___DATE___.
//___COPYRIGHT___
//

#ifndef ____PROJECTNAME_______FILEBASENAMEASIDENTIFIER____h_
#define ____PROJECTNAME_______FILEBASENAMEASIDENTIFIER____h_

#include <___PROJECTNAME___/___VARIABLE_superclass:identifier___.h>

/**
 * @file
 *
 * @brief ..
 */

typedef struct ___FILEBASENAMEASIDENTIFIER___ ___FILEBASENAMEASIDENTIFIER___;
typedef struct ___FILEBASENAMEASIDENTIFIER___Interface ___FILEBASENAMEASIDENTIFIER___Interface;

/**
 * @brief The ___FILEBASENAMEASIDENTIFIER___ type.
 *
 * @extends ___VARIABLE_superclass:identifier___
 *
 * @ingroup
 */
struct ___FILEBASENAMEASIDENTIFIER___ {
	
	/**
	 * @brief The parent.
	 *
	 * @private
	 */
	___VARIABLE_superclass:identifier___ ___VARIABLE_superclassMember:identifier___;
	
	/**
	 * @brief The typed interface.
	 *
	 * @private
	 */
	___FILEBASENAMEASIDENTIFIER___Interface *interface;
	
	//..
};

/**
 * @brief The ___FILEBASENAMEASIDENTIFIER___ interface.
 */
struct ___FILEBASENAMEASIDENTIFIER___Interface {
	
	/**
	 * @brief The parent interface.
	 */
	___VARIABLE_superclass:identifier___Interface ___VARIABLE_superclassMember:identifier___Interface;
	
	/**
	 * @fn ___FILEBASENAMEASIDENTIFIER___ *___FILEBASENAMEASIDENTIFIER___::init(___FILEBASENAMEASIDENTIFIER___ *self)
	 *
	 * @brief Initializes this ___FILEBASENAMEASIDENTIFIER___.
	 *
	 * @return The initialized ___FILEBASENAMEASIDENTIFIER___, or `NULL` on error.
	 *
	 * @memberof ___FILEBASENAMEASIDENTIFIER___
	 */
	___FILEBASENAMEASIDENTIFIER___ *(*init)(___FILEBASENAMEASIDENTIFIER___ *self);
	
	// ..
};

/**
 * @brief The ___FILEBASENAMEASIDENTIFIER___ Class.
 */
extern Class ____FILEBASENAMEASIDENTIFIER___;

#endif /* ____PROJECTNAME_______FILEBASENAMEASIDENTIFIER____h_ */

