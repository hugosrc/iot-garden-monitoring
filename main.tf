terraform {
  backend "s3" {
    bucket  = "terraform-github-aws-tfstates"
    key     = "iot-garden-monitoring/terraform.tfstate"
    region  = "us-east-1"
    encrypt = true
  }

  required_providers {
    aws = {
      source  = "hashicorp/aws"
      version = "~> 5.0"
    }
  }
}

provider "aws" {
  region = var.aws_region
}
