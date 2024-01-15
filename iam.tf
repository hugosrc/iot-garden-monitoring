resource "aws_iam_role" "lambda_role" {
  name = "iot_monitoring_lambda_role"
  assume_role_policy = jsonencode({
    "Version" : "2012-10-17",
    "Statement" : [
      {
        "Action" : "sts:AssumeRole",
        "Principal" : {
          "Service" : "lambda.amazonaws.com"
        },
        "Effect" : "Allow",
        "Sid" : ""
      }
    ]
  })
}

resource "aws_iam_policy" "iam_policy_for_lambda" {
  name        = "iot_monitoring_lambda_policy"
  path        = "/"
  description = "IAM Policy for AWS Lambda in IoT Monitoring"
  policy = jsonencode({
    "Version" : "2012-10-17",
    "Statement" : [
      {
        "Action" : [
          "logs:CreateLogGroup",
          "logs:CreateLogStream",
          "logs:PutLogEvents",
        ],
        "Resource" : "arn:aws:logs:*:*:*",
        "Effect" : "Allow"
      },
      {
        "Action" : [
          "dynamodb:PutItem",
        ],
        "Resource" : "arn:aws:dynamodb:*:*:table/*",
        "Effect" : "Allow"
      }
    ]
  })
}

resource "aws_iam_role_policy_attachment" "attach_iam_policy_to_iam_role" {
  role       = aws_iam_role.lambda_role.name
  policy_arn = aws_iam_policy.iam_policy_for_lambda.arn
}