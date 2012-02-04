 -------------------------------------------------------------------------
 |||||||||||||||||||||||||||||| RULES FORMS ||||||||||||||||||||||||||||||
 -------------------------------------------------------------------------

 maintained by Jordan Halterman <jordan.halterman@gmail.com>
 http://drupal.org/project/rules_forms

 Thanks to klausi and fago for original development and maintainance of
 the earlier version of Rules Forms, which was included with Rules 1.

 Installation
 ------------
 This module requires Rules 2 (http://drupal.org/project/rules).
 Navigate to administer >> module and enable the Rules Forms module.


 Getting started
 ---------------
 This is a short usage guide to build Rules on you forms:

 * Go to the "Form events" page in the Rules administration menu
   (admin/config/workflow/rules/forms).
 * Select the checkbox "Enable event activation messages on forms" and hit the "Save
   settings" button.
 * Go to the form on your site that you would like to customize with Rules, e.g.
   go to 'node/add/story' to enable events on the "Create Story" form.
 * On the top of the page you see a drupal message with a link to activate events
   for the form, click it.
 * Confirm the activation by clicking the "Activate" button.
 * Go to the Rules admin page (admin/config/workflow/rules) and click the "Add
   new rule" menu item.
 * Fill out the label, choose a form event by selecting one in the "Rules forms"
   group and confirm with "Save changes".
 * Now you can add conditions and actions to react on the form event.


 Form element conditions and actions
 -----------------------------------

 The Rules forms module allows you to manipulate single form elements, where you
 need the ID of the element. This guide shows you you how to find them.

 * Go to the "Form events" page in the Rules administration menu
   (admin/config/workflow/rules/forms).
 * Make sure that you have activated events on your target form, it should be listed
   on the page.
 * Add a condition or action to your rule.
 * If the condition or action requires a form element to be targeted, you will be
   presented a select-list of known form elements for the form.
 * For extra information or to help in identifying form elements, you can activate
   form element information in the Rules forms settings. See below for more details.


 Using the "Set element properties" action
 -----------------------------------------
 This is a short usage guide to build actions with the "Set any property" action:

 * Go to the "Form events" page in the Rules administration menu
   (admin/config/workflow/rules/forms).
 * Enable form element property information by activating the checkbox labeled
   "Display form element information".
 * Navigate back to Rules administration (admin/config/workflow/rules).
 * Create a rule using an event of one of your activated forms.
 * To use the "set any property" action, select "Add action" and choose the
   "Set element properties" action under "Rules forms".
 * At the next form, select the desired form element from the drop-down list.
 * As a reference, you can navigate back to the form whose event you are using.
   By mousing over or clicking the "i" information icon next to any element you
   can view a list of element properties. The title of the popup list will show
   Rules forms' name of that element.
 * Once a form element is selected, press "continue".
 * You will be provided a text area in which to enter element properties and values.
   This field accepts tokens that represent properties to set. Also provided is a
   token list of element properties that can be set.
 * To set a property, select the "Property" field and click the desired property
   token in the token list. Alternatively, you can enter the token text directly.
 * To set the value of the property, enter an equals (=) sign after the element
   property token followed by the desired value. For multiple value settings
   separate values by a comma (,).
 * To set multiple properties, enter one property setting per line.
 * Example:
   [element:title] = Example!
   [element:options] = apples, bananas, oranges, pears
