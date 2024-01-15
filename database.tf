resource "aws_dynamodb_table" "iot_monitoring_table" {
  name           = "IoTMonitoringGardenMetrics"
  billing_mode   = "PROVISIONED"
  read_capacity  = 1
  write_capacity = 1
  hash_key       = "GardenID"
  range_key      = "Timestamp"

  attribute {
    name = "GardenID"
    type = "S"
  }

  attribute {
    name = "Timestamp"
    type = "S"
  }
}