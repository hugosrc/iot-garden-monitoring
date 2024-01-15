data "archive_file" "lambda_function_zip" {
  type        = "zip"
  source_dir  = "${path.module}/api"
  output_path = "${path.module}/api/lambda_function.zip"
}

resource "aws_lambda_function" "iot_monitoring_lambda" {
  filename      = "${path.module}/api/lambda_function.zip"
  function_name = "iot_monitoring_lambda_function"
  role          = aws_iam_role.lambda_role.arn
  handler       = "index.handler"
  runtime       = "nodejs20.x"
  depends_on    = [aws_iam_role_policy_attachment.attach_iam_policy_to_iam_role]
}

resource "aws_iot_topic_rule" "iot_rule" {
  name        = "esp32_garden_monitoring_rule"
  enabled     = true
  sql         = "SELECT * FROM \"iot/esp32/garden/monitoring\""
  sql_version = "2015-10-08"

  lambda {
    function_arn = aws_lambda_function.iot_monitoring_lambda.arn
  }
}

data "aws_iam_policy_document" "iam_invoke_lambda_policy_document" {
  statement {
    actions   = ["lambda:InvokeFunction"]
    resources = ["*"]
  }
}

resource "aws_lambda_permission" "decommission_access_topic_rule_permission" {
  action        = "lambda:InvokeFunction"
  function_name = aws_lambda_function.iot_monitoring_lambda.function_name
  principal     = "iot.amazonaws.com"
  source_arn    = aws_iot_topic_rule.iot_rule.arn
}