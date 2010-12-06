<?php

// $Id$

/**
 * @file
 *   Pages and page callbacks
 *
 * @ingroup sagepayapi
 */

/**
 * Administrative settings form
 * @todo why does this form go to a blank page after submission?
 */
function sagepayapi_admin_settings() {
  $form['sagepayapi_form_url'] = array(
    '#type' => 'radios',
    '#title' => t('Sagepay Form URL'),
    '#description' => t('Select the Sagepay Form server to use. Be CAREFUL with this!'),
    '#default_value' => variable_get('sagepayapi_form_url', 'SAGEPAYAPI_FORM_SIMULATOR_URL'),
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
    '#description' => t('This is the vendor id you set up with or were assigned by Sagepay'),
    '#default_value' => variable_get('sagepayapi_vendor', ''),
    '#required' => TRUE,
  );

  $form['sagepayapi_encrypted_pass'] = array(
    '#type' => 'password',
    '#title' => t('XOR Encryption Password'),
    '#description' => t('This is the XOR encrypted password supplied by Sagepay'),
    //'#default_value' => variable_get('sagepayapi_encrypted_pass', ''),
    //'#required' => TRUE,
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

  $form['sagepayapi_currencies'] = array(
    '#type' => 'select',
    '#title' => t('Currency'),
    '#description' => t('Select all of the currencies in which you wish to trade.'),
    '#multiple' => TRUE,
    '#default_value' => variable_get('sagepayapi_currencies', array('GBP' => 'GBP')),
    '#options' => array(
      'GBP' => 'GBP',
      'USD' => 'USD',
      'EUR' => 'EUR',
      'AUD' => 'AUD',
    ),
  );

  $form['sagepayapi_transaction_type'] = array(
    '#type' => 'radios',
    '#title' => t('Transaction Type'),
    '#description' => t('Depends upon what your Sage Pay account supports.'),
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
    '#title' => t('Optional Settings'),
    '#collapsible' => TRUE,
  );

  $form['sagepayapi_optional_settings']['sagepayapi_send_email'] = array(
    '#type' => 'radios',
    '#title' => t('Email Options'),
    '#default_value' => variable_get('sagepayapi_send_email', 1),
    '#options' => array(
      0 => t('Do not send either customer or vendor e-mails'),
      1 => t('Send customer and vendor e-mails if address(es) are provided (DEFAULT).'),
      2 => t('Send Vendor Email but not Customer Email. If you do not supply this field, 1 is assumed and e-mails are sent if addresses are provided.'),
    ),
  );

  $form['sagepayapi_optional_settings']['sagepayapi_vendor_email'] = array(
    '#type' => 'textfield',
    '#title' => t('Vendor Email Address'),
    '#description' => t('Optional setting. Set this to the mail address which will receive order confirmations and failures.'),
    '#default_value' => variable_get('sagepayapi_vendor_email', ''),
  );

  $form['sagepayapi_optional_settings']['sagepayapi_partner_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Partner ID'),
    '#description' => t('If you are a Sage Pay Partner and wish to flag the transactions with your unique partner id set it here.'),
    '#default_value' => variable_get('sagepayapi_partner_id', ''),
  );

  $form['#validate'][] = 'sagepayapi_admin_settings_validate';
  return system_settings_form($form);
}

/**
 * Validate the administrative settings form
 */
function sagepayapi_admin_settings_validate($form, &$form_state) {
  if ($form_state['values']['sagepayapi_success_page']) {
    $path = trim(check_plain($form_state['values']['sagepayapi_success_page']));
    $ex = menu_get_item(drupal_get_normal_path($path));
    if (!$ex) {
      form_set_error('sagepayapi_success_page', t('Success page is not a valid Drupal path.'));
    }
  }
  if ($form_state['values']['sagepayapi_failure_page']) {
    $path = trim(check_plain($form_state['values']['sagepayapi_failure_page']));
    $ex = menu_get_item(drupal_get_normal_path($path));
    if (!$ex) {
      form_set_error('sagepayapi_failure_page', t('Failure page is not a valid Drupal path.'));
    }
  }
  // If the XOR password field is blank, we don't want to overwrite what may already be saved.
  if (empty($form_state['values']['sagepayapi_encrypted_pass'])) {
    unset($form_state['values']['sagepayapi_encrypted_pass']);
  }
}

/**
 * This form is a two-step form
 */
function sagepayapi_form_order_form($form_state) {
  // @todo enhance this for if client just needs page two
  if (isset($form_state['storage']['page_two'])) {
    //$order = sagepayapi_dummy_order();
    $values = $form_state['storage']['page_one_values'];
    // Build order info for Sage Pay
    $billingcountry = check_plain($values['BillingCountry']);
    $billingaddress2 = check_plain($values['BillingAddress2']);
    
    $order = array(
      'Description' => t('Internship Application'), // @todo make this configurable
      //'Amount' => '1', // @todo make this configurable from a hidden form field or something?
      'Currency' => check_plain($values['Currency']),
      'BillingSurname' => check_plain($values['BillingSurname']),
      'BillingFirstnames' => check_plain($values['BillingFirstnames']),
      //'CustomerEmail' => "stacey.borsody@yahoo.com",
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

    $currency_symbol = '';
    
    // Pick correct amount for application fee based upon selected currency.
    switch ($order['Currency']) {
      default:
      case 'GBP':
        $order['Amount'] = '50';
        $currency_symbol = "&pound;"; // £
        break;
      case 'USD':
        $order['Amount'] = '75';
        $currency_symbol = "$";
        break;
      case 'EUR':
        $order['Amount'] = '65';
        $currency_symbol = "&#8364;"; // €
        break;
      case 'AUD':
        $order['Amount'] = '100';
        $currency_symbol = "$";
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

    // Build a success url
    // Build a failure url
    $returnurl = url('sagepayapi/thankyou', array('absolute' => TRUE));
    $order['SuccessURL'] = $returnurl;
    $order['FailureURL'] = $returnurl;

    // @todo Somewhere here we need to verify that the order array has all we need.
    // need to add vendortxcode, amount, successurl, failureurl
    $order['VendorTxCode'] = $vendortxcode;

    // Create encrypted post string
    $crypt = sagepayapi_form_get_crypt_string($order);

    // Specify to which sagepay url this form should be submitted
    switch (variable_get('sagepayapi_form_url', '')) {
      case 'SAGEPAYAPI_FORM_SIMULATOR_URL':
        $form['#action'] = SAGEPAYAPI_FORM_SIMULATOR_URL;
        break;
      case 'SAGEPAYAPI_FORM_TEST_URL':
        $form['#action'] = SAGEPAYAPI_FORM_TEST_URL;
        break;
      case 'SAGEPAYAPI_FORM_LIVE_URL':
        $form['#action'] = SAGEPAYAPI_FORM_LIVE_URL;
        break;
      default:
        $form['#action'] = SAGEPAYAPI_FORM_SIMULATOR_URL;
        break;
    }

    // Provide feedback on what the order is.  This gets themed.
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
    $form['review']['basket'] = array('#title' => t('Application Fee'), '#value' => $order['Currency'] . " $currency_symbol" . $order['Amount']);

    // Create four hidden form fields
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
      '#value' => t('Pay Now'), // @todo make this configurable
    );

    // @todo we should probably unset the saved values from page one here
    unset($form_state['storage']['page_two']);
    unset($form_state['storage']['page_one_values']);
    return $form;
  }

  // @todo application fee (submitted with application by user through bounce from app form after submit): 50GBP, US$75 €65, AUD$100

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
  $form['billing']['BillingState'] = array(
    '#type' => 'select',
    '#title' => t('State (Required only for US)'),
    '#options' => _sagepayapi_get_states(),
  );
  $form['billing']['BillingCountry'] = array(
    '#type' => 'select',
    '#title' => t('Country'),
    '#required' => TRUE,
    '#default_value' => 'GB',
    '#options' => _sagepayapi_get_countries(),
  );
  $form['billing']['Currency'] = array(
    '#type' => 'select',
    '#title' => t('Currancy'),
    '#required' => TRUE,
    '#options' => variable_get('sagepayapi_currencies', array('GBP' => 'GBP')), // @todo detect default from billing country?
  );
  $form['BillingPhone'] = array(
    '#type' => 'textfield',
    '#title' => t('Phone number'),
    '#required' => TRUE,
  );

  $form['next'] = array(
    '#type' => 'submit',
    '#value' => t('Next'),
  );

  return $form;
}

/**
 * Validates data in billing order form
 */
function sagepayapi_form_order_form_validate($form, &$form_state) {
}

/**
 * Handles submission of order form
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
 */
function sagepayapi_form_transaction_status() {
  $output = '';

  $returnvalue = _sagepayapi_simpleXor(_sagepayapi_base64_decode($_GET['crypt']), variable_get('sagepayapi_encrypted_pass', ''));
  $tokens = _sagepayapi_get_tokens($returnvalue);

  // Log to watchdog
  watchdog('sagepayapi', 'Receiving notification. Return value: !raw', array('!raw' => check_plain($returnvalue)));

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
      watchdog('sagepayapi', 'MALFORMED error from Protx for order !order_id.', array('!order_id' => $order_protx->order_id));
      drupal_set_message(t('A system error occurred: Malformed post data.  This error has been logged and will be investigated.  Please contact us to confirm the status of your order'), 'warning');
      sagepayapi_form_status_goto(FALSE);
      break;
    case 'INVALID' : // Transaction not completed; can be retried.
      watchdog('sagepayapi', 'INVALID error from Protx for order !order_id.', array('!order_id' => $order_protx->order_id));
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
      watchdog('sagepayapi', 'Transaction was AUTHENTICATED. Vendor TX Code: !vendortxcode VPSTxId: !VPSTxId', array('!vendortxcode' => $vendortxcode, '!VPSTxId' => $vps_tx_id));
      drupal_set_message(t('Your payment information has been authenticated with Sage Pay.'));
      sagepayapi_form_status_goto(TRUE);
      break;
    case 'REGISTERED': // Only used with AUTHENTICATE TxType
      watchdog('sagepayapi', 'Transaction was REGISTERED. Vendor TX Code: !vendortxcode VPSTxId: !VPSTxId', array('!vendortxcode' => $vendortxcode, '!VPSTxId' => $vps_tx_id));
      drupal_set_message(t('Your payment information has been registered with Sage Pay.'));
      sagepayapi_form_status_goto(TRUE);
      break;
    case 'ERROR' : // Transaction not completed; can be retried. Please notify Sage Pay with VendorTxCode and StatusDetail if you see this error.
      watchdog('sagepayapi', 'System ERROR from Sage Pay for vendor tx code !vendortxcode with status details !statusdetail.', array('!vendortxcode' => $vendortxcode, '!statusdetails' => $statusdetail), WATCHDOG_ERROR);
      drupal_set_message(t('A problem occurred at Sage Pay which prevented transaction completion.  This error has been logged and will be investigated.  Please contact us to confirm your order status.'), 'error');
      sagepayapi_form_status_goto(FALSE);
      break;
  }

  return $output;
}