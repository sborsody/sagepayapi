Sage Pay API for Drupal 6
=========================

This module provides a programming interface to Sage Pay for Drupal.
Currently only Sage Pay Form is implemented.  Version 1.x of this module is
customized for a client.

The customized payment form is found at /online-payment.
The settings page can be found at /admin/settings/sagepayapi.

To use:
  1. Go to the settings page
  2. Select either Simulator, Test, or Live for the Sage Pay Form URL.
  3. Set your Sage Pay Vendor ID, your XOR password (it is different from
      your Sage Pay web password).
  4. Give the paths to your site's success and failure pages, minus the leading
      slash (/).
  5. Only the Payment transaction type is supported at this time.
  6. Under Optional Settings, you can select the Sage Pay email behavior.
      Sage Pay defaults to send email if email addresses are provided.

Changelog

6.x-1.1
  - Modified form for client

6.x-1.0
  - Final customer-specific release (hopefully)

6.x-1.0-beta4
  - Added configurable amounts
  - Added ctools dependency to billing state
  - Minor title and copy edits

6.x-1.0-beta3
  - Added better currency handling and currency format validation for
    user-entered amount

6.x-1.0-beta2
  - Added deposit fee and balance payoff fee "product".

6.x-1.0-beta1
  - Initial version.  Application fee was only "product".
