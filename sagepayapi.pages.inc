<?php

// $Id$

/**
 * @file
 * Pages and page callbacks
 *
 * @ingroup sagepayapi
 */

/**
 * Form builder for the Sage Pay administrative settings form.
 *
 * @see sagepayapi_admin_settings_form_validate()
 * @ingroup forms
 */
function sagepayapi_admin_settings_form() {
  $form['sagepayapi_form_url'] = array(
    '#type' => 'radios',
    '#title' => t('Sage Pay Form URL'),
    '#description' => t('Select the Sage Pay Form server to use. Be CAREFUL with this!'),
    '#default_value' => variable_get('sagepayapi_form_url', SAGEPAYAPI_FORM_SIMULATOR_URL),
    '#options' => array(
      'SAGEPAYAPI_FORM_SIMULATOR_URL' => t("Simulator"),
      'SAGEPAYAPI_FORM_TEST_URL' => t("Test"),
      'SAGEPAYAPI_FORM_LIVE_URL' => t("Live"),
    ),
    '#required' => TRUE,
  );

  $form['sagepayapi_vendor'] = array(
    '#type' => 'textfield',
    '#title' => t('Vendor'),
    '#description' => t('This is the vendor ID you set up with or were assigned by Sage Pay'),
    '#default_value' => variable_get('sagepayapi_vendor', ''),
    '#required' => TRUE,
  );

  $form['sagepayapi_encrypted_pass'] = array(
    '#type' => 'password',
    '#title' => t('XOR encryption password'),
    '#description' => t('This is the XOR encrypted password supplied by Sage Pay'),
  );

  $form['sagepayapi_success_page'] = array(
    '#type' => 'textfield',
    '#title' => t("Your site's transaction success page"),
    '#description' => t('This should be a Drupal path.'),
    '#default_value' => variable_get('sagepayapi_success_page', ''),
  );

  $form['sagepayapi_failure_page'] = array(
    '#type' => 'textfield',
    '#title' => t("Your site's transaction failure page"),
    '#description' => t('This should be a Drupal path.'),
    '#default_value' => variable_get('sagepayapi_failure_page', ''),
  );

  // Get currencies
  $currencies = _sagepayapi_get_currencies();
  foreach ($currencies as $key => $value) {
    $cur_options[$key] = $value['name'];
  }

  $set_curs = variable_get('sagepayapi_currencies', '');

  $form['sagepayapi_currencies'] = array(
    '#type' => 'select',
    '#title' => t('Currency'),
    '#description' => t('Select all of the currencies in which you wish to trade.'),
    '#multiple' => TRUE,
    '#default_value' => $set_curs,
    '#options' => $cur_options,
  );

  $form['sagepayapi_amounts'] = array(
    '#type' => 'fieldset',
    '#title' => t('Amounts'),
    '#description' => t('Amounts for application fee and deposit fee in each currency'),
    '#tree' => TRUE,
    '#theme' => 'sagepayapi_admin_amounts',
  );


  foreach ($set_curs as $key => $value) {
      $arr = array(
      '#type' => 'textfield',
      '#size' => 6,
    );

    $app = t('Application Fee');
    $dep = t('Deposit Fee');
    $def_var = variable_get('sagepayapi_amounts', array());

    $form['sagepayapi_amounts']['APP'][$key] = $arr;
    $form['sagepayapi_amounts']['APP'][$key]['#default_value'] = $def_var['APP'][$key];
    $form['sagepayapi_amounts']['APP'][$key]['#field_prefix'] = $key . ' ' . $currencies[$key]['symbol'];
    // $form['sagepayapi_amounts']['APP'][$key]['#prefix'] = $app;

    $form['sagepayapi_amounts']['DEP'][$key] = $arr;
    $form['sagepayapi_amounts']['DEP'][$key]['#default_value'] = $def_var['DEP'][$key];
    $form['sagepayapi_amounts']['DEP'][$key]['#field_prefix'] = $key . ' ' . $currencies[$key]['symbol'];
    // $form['sagepayapi_amounts']['DEP'][$key]['#prefix'] = $dep;
  }

  $form['sagepayapi_transaction_type'] = array(
    '#type' => 'radios',
    '#title' => t('Transaction type'),
    '#description' => t('Depends upon what your Sage Pay account supports. (Deferred and authenticate are not supported at this time)'),
    '#default_value' => variable_get('sagepayapi_transaction_type', 'PAYMENT'),
    '#options' => array(
      'PAYMENT' => t("PAYMENT"),
      'DEFERRED' => t("DEFERRED"),
      'AUTHENTICATE' => t("AUTHENTICATE")
    ),
  );

  // Optional Settings Fieldset
  $form['sagepayapi_optional_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Optional settings'),
    '#collapsible' => TRUE,
  );

  $form['sagepayapi_optional_settings']['sagepayapi_send_email'] = array(
    '#type' => 'radios',
    '#title' => t('Email options'),
    '#default_value' => variable_get('sagepayapi_send_email', 1),
    '#options' => array(
      0 => t('Do not send either customer or vendor e-mails'),
      1 => t('Send customer and vendor e-mails if address(es) are provided (DEFAULT).'),
      2 => t('Send Vendor Email but not Customer Email. If you do not supply this field, 1 is assumed and e-mails are sent if addresses are provided.'),
    ),
  );

  $form['sagepayapi_optional_settings']['sagepayapi_vendor_email'] = array(
    '#type' => 'textfield',
    '#title' => t('Vendor email address'),
    '#description' => t('Optional setting. Set this to the mail address which will receive order confirmations and failures.'),
    '#default_value' => variable_get('sagepayapi_vendor_email', ''),
  );

  $form['sagepayapi_optional_settings']['sagepayapi_partner_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Partner ID'),
    '#description' => t('If you are a Sage Pay Partner and wish to flag the transactions with your unique partner id set it here.'),
    '#default_value' => variable_get('sagepayapi_partner_id', ''),
  );

  $form['sagepayapi_optional_settings']['sagepayapi_debug'] = array(
    '#type' => 'checkbox',
    '#title' => t('Watchdog debugging'),
    '#description' => t("Check this in order to send debugging information to Drupal's watchdog"),
    '#default_value' => variable_get('sagepayapi_debug', 'FALSE'),
  );

  $form['sagepayapi_optional_settings']['sagepayapi_form_submit_string'] = array(
    '#type' => 'textfield',
    '#title' => t('Sage Pay Form submit button text'),
    '#description' => t('The text to display on the Sage Pay Form submit button'),
    '#default_value' => variable_get('sagepayapi_form_submit_string', t('Pay Now')),
  );

  $form['#validate'][] = 'sagepayapi_admin_settings_form_validate';
  return system_settings_form($form);
}

/**
 * Form validation handler for sagepayapi_admin_settings_form()
 *
 * @see sagepayapi_admin_settings_form()
 */
function sagepayapi_admin_settings_form_validate($form, &$form_state) {
  // @todo refactor this check for both success and failure pages
  if ($form_state['values']['sagepayapi_success_page']) {
    $path = trim(check_plain($form_state['values']['sagepayapi_success_page']));
    $ex = menu_get_item(drupal_get_normal_path($path));
    if (!$ex) {
      form_set_error('sagepayapi_success_page', t('Success page is not a valid Drupal path.'));
    }
  }

  // Ensure that this is a valid path
  if ($form_state['values']['sagepayapi_failure_page']) {
    $path = trim(check_plain($form_state['values']['sagepayapi_failure_page']));
    $ex = menu_get_item(drupal_get_normal_path($path));
    if (!$ex) {
      form_set_error('sagepayapi_failure_page', t('Failure page is not a valid Drupal path.'));
    }
  }

  // Ensure that at least one currency is selected
  if (empty($form_state['values']['sagepayapi_currencies'])) {
    form_set_error('sagepayapi_currencies', t('Please select at least one currency.'));
  }

  // Since we do not display a default value, check if the password exists already and do not remove it
  if (empty($form_state['values']['sagepayapi_encrypted_pass'])) {
    if (variable_get('sagepayapi_encrypted_pass', '')) {
      unset($form_state['values']['sagepayapi_encrypted_pass']);
    }
    else {
      form_set_error('sagepayapi_encrypted_pass', t('Please enter the XOR encryption password provided by Sage Pay.'));
    }
  }
}

/**
 * Form builder for the Sage Pay Form order form.
 *
 * @see sagepayapi_form_order_form_validate()
 * @see sagepayapi_form_order_form_submit()
 * @ingroup forms
 */
function sagepayapi_form_order_form($form_state, $req = NULL) {
  // Set up fee schedule
  // @todo make amounts configurable
  $amounts = variable_get('sagepayapi_amounts', array());
  $fees = array(
    'APP' => array(
      '#name' => t('Internship Application'),
      '#url' => 'application', // @todo make this a constant?
      '#amounts' => $amounts['APP'],
    ),
    'DEP' => array(
      '#name' => t('Program Deposit'),
      '#url' => 'deposit',
      '#amounts' => $amounts['DEP'],
    ),
    'OTH' => array(
      '#name' => t('Program Balance (Enter amount below)'),
      '#url' => 'balance',
      // No amounts because this will be textfield
    ),
  );
  $request = check_plain($req); // This is important! @todo test that someone can't type something funny in the url
  $allowed_requests = array();
  $fee_options = array();
  // Figure out what page argument strings are allowed and what their friendly names are
  foreach ($fees as $key => $value) {
    $allowed_requests[$value['#url']] = $key;
    $fee_options[$key] = $value['#name'];
  }
  // Check if the page argument string is allowed
  $allowed = array_key_exists($request, $allowed_requests);
  // Set the correct request name
  if ($allowed) {
    $description = $fee_options[$allowed_requests[$request]];
  }
  // Default to the APP request name
  else {
    $description = $fee_options['APP'];
  }

  $currencies = _sagepayapi_get_currencies();
  $valid_currencies = variable_get('sagepayapi_currencies', '');
  foreach ($valid_currencies as $cur) {
    $cur_options[$cur] = $currencies[$cur]['name'];
  }

  // Page two of this form
  // @todo enhance this for if client just needs page two
  if (isset($form_state['storage']['page_two'])) {
    // $order = sagepayapi_dummy_order();
    $values = $form_state['storage']['page_one_values'];
    // Build order info for Sage Pay
    $billingcountry = check_plain($values['BillingCountry']);
    $billingaddress2 = check_plain($values['BillingAddress2']);
      // @todo fix this
    $cur_code = check_plain($values['Currency']);
    $currency = _sagepayapi_get_currency($cur_code);

    $order = array(
      'Description' => $description,
      'Currency' => check_plain($values['Currency']),
      'BillingSurname' => check_plain($values['BillingSurname']),
      'BillingFirstnames' => check_plain($values['BillingFirstnames']),
      'BillingAddress1' => check_plain($values['BillingAddress1']),
      'BillingCity' => check_plain($values['BillingCity']),
      'BillingPostCode' => check_plain($values['BillingPostCode']),
      'BillingCountry' => $billingcountry,
      'BillingPhone' => check_plain($values['BillingPhone']),
      'DeliverySurname' => check_plain($values['BillingSurname']),
      'DeliveryFirstnames' => check_plain($values['BillingFirstnames']),
      'DeliveryAddress1' => check_plain($values['BillingAddress1']),
      'DeliveryCity' => check_plain($values['BillingCity']),
      'DeliveryPostCode' => check_plain($values['BillingPostCode']),
      'DeliveryCountry' => $billingcountry,
    );

    // Pick correct amount for application fee based upon selected currency.
    // @todo make this configurable
    if ($request && $allowed) {
      if ($request == 'balance') {
        $order['Amount'] = check_plain($values['UserAmount']);
      }
      else {
        $order['Amount'] = $fees[$allowed_requests[$request]]['#amounts'][$order['Currency']];
      }
    }
    // Default to APP
    else {
        $order['Amount'] = $fees['APP']['#amounts'][$order['Currency']];
    }

    // Only set State if country is US
    if ($billingcountry == 'US') {
      $billingstate = check_plain($values['BillingState']);
      $order['BillingState'] = $billingstate;
      $order['DeliveryState'] = $billingstate;
    }
    if ($billingaddress2) {
      $order['BillingAddress2'] = $billingaddress2;
      $order['DeliveryAddress2'] = $billingaddress2;
    }

    // Generate vendor tx code
    $vendortxcode = sagepayapi_get_vendortxcode();

    // Build a success and failure url
    $returnurl = url('sagepayapi/thankyou', array('absolute' => TRUE));
    $order['SuccessURL'] = $returnurl;
    $order['FailureURL'] = $returnurl;

    // @todo Somewhere here we need to verify that the order array has all we need.
    // need to add vendortxcode, amount, successurl, failureurl
    $order['VendorTxCode'] = $vendortxcode;

    // Specify the required Sage Pay Form url to which this form gets submitted
    switch (variable_get('sagepayapi_form_url', '')) {
      case 'SAGEPAYAPI_FORM_LIVE_URL':
        $form['#action'] = SAGEPAYAPI_FORM_LIVE_URL;
        break;
      case 'SAGEPAYAPI_FORM_TEST_URL':
        $form['#action'] = SAGEPAYAPI_FORM_TEST_URL;
        break;
      case 'SAGEPAYAPI_FORM_SIMULATOR_URL':
      default:
        $form['#action'] = SAGEPAYAPI_FORM_SIMULATOR_URL;
        break;
    }

    // Provide feedback on what the order is.  This gets themed.
    // @todo make the display of this feedback optional for carts that don't need it
    $form['review'] = array(
      '#type' => 'fieldset',
      '#title' => t('Order Review'),
      '#tree' => TRUE,
      '#theme' => 'sagepayapi_form_order_review',
    );
    $form['review']['firstname'] = array('#title' => t('First name'), '#value' => $order['BillingFirstnames']);
    $form['review']['surname'] = array('#title' => t('Last name'), '#value' => $order['BillingSurname']);
    $form['review']['address1'] = array('#title' => t('Street Address (Line 1)'), '#value' => $order['BillingAddress1']);
    if ($billingaddress2) {
      $form['review']['address2'] = array('#title' => t('Street Address (Line 2)'), '#value' => $billingaddress2);
    }
    $form['review']['city'] = array('#title' => t('City'), '#value' => $order['BillingCity']);
    $form['review']['postcode'] = array('#title' => t('Postal/ZIP Code'), '#value' => $order['BillingPostCode']);
    $form['review']['country'] = array('#title' => t('Country'), '#value' => $billingcountry);
    if ($billingstate) {
      $form['review']['state'] = array('#title' => t('State'), '#value' => $billingstate);
    }
    $form['review']['basket'] = array('#title' => $description, '#value' => $cur_code . " " . $currency['symbol'] . $order['Amount']);

    // Create the four hidden form fields required by Sage Pay Form
    $hidden_fields = array(
      'VPSProtocol' => sagepayapi_protocol_version(),
      'TxType' => variable_get('sagepayapi_transaction_type', 'PAYMENT'),
      'Vendor' => variable_get('sagepayapi_vendor', ''),
      'Crypt' => sagepayapi_form_get_crypt_string($order),
    );

    // Generate the hidden form fields
    foreach ($hidden_fields as $name => $value) {
      $form[$name] = array('#type' => 'hidden', '#value' => $value);
    }

    // Create submit button
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => variable_get('sagepayapi_form_submit_string', t('Pay Now')),
    );

    // @todo we should probably unset the saved values from page one here. does this do anything?
    unset($form_state['storage']['page_two']);
    unset($form_state['storage']['page_one_values']);
    return $form;
  }

  // Page one of this form
  $form['BillingFirstnames'] = array(
    '#type' => 'textfield',
    '#title' => t('First name'),
    '#description' => t('Please enter the first name of the person to be billed.'),
    '#required' => TRUE,
  );
  $form['BillingSurname'] = array(
    '#type' => 'textfield',
    '#title' => t('Last name'),
    '#description' => t('Please enter the last name of the person to be billed.'),
    '#required' => TRUE,
  );
  $form['billing'] = array(
    '#type' => 'fieldset',
    '#title' => t('Billing information'),
  );
  $form['billing']['BillingAddress1'] = array(
    '#type' => 'textfield',
    '#title' => t('Street address (Line 1)'),
    '#required' => TRUE,
  );
  $form['billing']['BillingAddress2'] = array(
    '#type' => 'textfield',
    '#title' => t('Street address (Line 2)'),
  );
  $form['billing']['BillingCity'] = array(
    '#type' => 'textfield',
    '#title' => t('City'),
    '#required' => TRUE,
  );
  $form['billing']['BillingPostCode'] = array(
    '#type' => 'textfield',
    '#title' => t('Postal/ZIP Code'),
    '#required' => TRUE,
  );

  $form['billing']['BillingCountry'] = array(
    '#type' => 'select',
    '#title' => t('Country'),
    '#required' => TRUE,
    '#default_value' => 'GB',
    '#options' => _sagepayapi_get_countries(),
  );
  ctools_include('dependent');
  $form['billing']['BillingState'] = array(
    '#type' => 'select',
    '#title' => t('State (Required only for US)'),
    '#options' => _sagepayapi_get_states(),
    '#process' => array('ctools_dependent_process'),
    '#dependency' => array(
      'edit-BillingCountry' => array('US'),
    ),
  );

  $form['billing']['BillingPhone'] = array(
    '#type' => 'textfield',
    '#title' => t('Phone number'),
    '#required' => TRUE,
  );

  $form['order'] = array(
    '#type' => 'fieldset',
    '#title' => t('Fee information'),
  );

  $form['order']['Currency'] = array(
    '#type' => 'select',
    '#title' => t('Currency'),
    '#required' => TRUE,
    '#default_value' => 'GBP',
    '#options' => $cur_options, // @todo detect default from billing country?
  );

  // Default to default description
  $form['order']['Description'] = array(
    '#value' => $description,
  );
  // Show the amount textfield for balance payoff requests
  if (isset($request) && $request == 'balance') { // @todo replace with constant
    $form['order']['UserAmount'] = array(
      '#type' => 'textfield',
      '#title' => t('Enter amount'),
      '#description' => t('Please enter a number without the currency symbol for the remaining program balance. Example: 23.00'),
    );
  }

  $form['next'] = array(
    '#type' => 'submit',
    '#value' => t('Next'),
  );

  return $form;
}

/**
 * Form validation handler for sagepayapi_form_order_form()
 *
 * @see sagepayapi_form_order_form()
 * @see sagepayapi_form_order_form_submit()
 */
function sagepayapi_form_order_form_validate($form, &$form_state) {
  // Validate page one form submission
  if ($form_state['clicked_button']['#id'] == 'edit-next') {
    if ($form_state['values']['UserAmount']) {
      $ua = trim(check_plain($form_state['values']['UserAmount']));
      $cur_code = check_plain($form_state['values']['Currency']);
      $currency = _sagepayapi_get_currency($cur_code);
      $pattern = $currency['pattern']; // Get the custom regexp pattern for this specific currency
      if (!is_numeric($ua) && !preg_match($pattern, $ua)) {
        form_set_error('UserAmount', t('Please enter an amount that is formatted correctly for this currency.'));
      }
    }
  }
}

/**
 * Form submission handler for sagepayapi_form_order_form()
 *
 * @see sagepayapi_form_order_form()
 * @see sagepayapi_form_order_form_validate()
 */
function sagepayapi_form_order_form_submit($form, &$form_state) {
  if ($form_state['clicked_button']['#id'] == 'edit-next') {
    $form_state['storage']['page_two'] = TRUE;
    $form_state['storage']['page_one_values'] = $form_state['values'];
  }
  // else {} We don't need to do anything else!  Page two gets processed by Sage Pay.
}

/**
 * Page callback for thank you page
 *
 * @todo check plain the get string!
 */
function sagepayapi_form_transaction_status() {
  $returnvalue = _sagepayapi_simpleXor(_sagepayapi_base64_decode($_GET['crypt']), variable_get('sagepayapi_encrypted_pass', ''));
  $tokens = _sagepayapi_get_tokens($returnvalue);

  // Log to watchdog @todo make this optional
  watchdog('sagepayapi', 'Sage Pay Form Receiving notification. Return value: !raw', array('!raw' => check_plain($returnvalue)), WATCHDOG_INFO);

  $status = check_plain($tokens['Status']);
  $vendortxcode = check_plain($tokens['VendorTxCode']);
  $statusdetail = check_plain($tokens['StatusDetail']);
  $vps_tx_id = check_plain($tokens['VPSTxId']);

  switch ($status) {
    case 'OK':
      drupal_set_message(t('Transaction completed sucessfully.'));
      sagepayapi_form_status_goto(TRUE);
      break;
    case 'NOTAUTHED' :
      watchdog('sagepayapi', 'Transaction was NOTAUTHED for order !vendortxcode VPSTxId: !VPSTxId', array('!vendortxcode' => $vendortxcode, '!VPSTxId' => $vps_tx_id), WATCHDOG_WARNING);
      drupal_set_message(t('The bank has declined your transaction a number of times so we cannot process your order at this time.  Please contact us to continue your order manually.'), 'warning');
      sagepayapi_form_status_goto(FALSE);
      break;
    case 'MALFORMED' : // Transaction not completed; can be retried.
      watchdog('sagepayapi', 'MALFORMED error from Protx for order !order_id.', array('!order_id' => $order_protx->order_id), WATCHDOG_WARNING);
      drupal_set_message(t('A system error occurred: Malformed post data.  This error has been logged and will be investigated.  Please contact us to confirm the status of your order'), 'warning');
      sagepayapi_form_status_goto(FALSE);
      break;
    case 'INVALID' : // Transaction not completed; can be retried.
      watchdog('sagepayapi', 'INVALID error from Protx for order !order_id.', array('!order_id' => $order_protx->order_id), WATCHDOG_WARNING);
      drupal_set_message(t('A system error occurred: Invalid data in post.  This error has been logged and will be investigated.  Please contact us to confirm the status of your order'), 'warning');
      sagepayapi_form_status_goto(FALSE);
      break;
    case 'ABORT': // Transaction not completed; can be retried.
      watchdog('sagepayapi', 'Transaction was ABORTed for order !vendortxcode', array('!vendortxcode' => $vendortxcode), WATCHDOG_WARNING);
      drupal_set_message(t('Your order was aborted during the payment process.  No payment has been made and you may repeat your order.'), 'warning');
      sagepayapi_form_status_goto(FALSE);
      break;
    case 'REJECTED' : // Bank authorized transaction but didn't pass your rules for AVS/CV2 or 3D-Secure
      watchdog('sagepayapi', 'Transaction was REJECTED', WATCHDOG_WARNING);
      drupal_set_message(t('Your payment could not be processed at this time.  Please contact us to complete your order manually'), 'warning');
      sagepayapi_form_status_goto(FALSE);
      break;
    case 'AUTHENTICATED': // Only used with AUTHENTICATE TxType
      watchdog('sagepayapi', 'Transaction was AUTHENTICATED. Vendor TX Code: !vendortxcode VPSTxId: !VPSTxId', array('!vendortxcode' => $vendortxcode, '!VPSTxId' => $vps_tx_id), WATCHDOG_INFO);
      drupal_set_message(t('Your payment information has been authenticated with Sage Pay.'));
      sagepayapi_form_status_goto(TRUE);
      break;
    case 'REGISTERED': // Only used with AUTHENTICATE TxType
      watchdog('sagepayapi', 'Transaction was REGISTERED. Vendor TX Code: !vendortxcode VPSTxId: !VPSTxId', array('!vendortxcode' => $vendortxcode, '!VPSTxId' => $vps_tx_id), WATCHDOG_INFO);
      drupal_set_message(t('Your payment information has been registered with Sage Pay.'));
      sagepayapi_form_status_goto(TRUE);
      break;
    case 'ERROR' : // Transaction not completed; can be retried. Please notify Sage Pay with VendorTxCode and StatusDetail if you see this error.
      watchdog('sagepayapi', 'System ERROR from Sage Pay for vendor tx code !vendortxcode with status details !statusdetail.', array('!vendortxcode' => $vendortxcode, '!statusdetails' => $statusdetail), WATCHDOG_ERROR);
      drupal_set_message(t('A problem occurred at Sage Pay which prevented transaction completion.  This error has been logged and will be investigated.  Please contact us to confirm your order status.'), 'error');
      sagepayapi_form_status_goto(FALSE);
      break;
  }
}