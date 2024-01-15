import { DynamoDBClient } from "@aws-sdk/client-dynamodb";
import { DynamoDBDocumentClient, PutCommand } from "@aws-sdk/lib-dynamodb";

const client = new DynamoDBClient({});
const docClient = DynamoDBDocumentClient.from(client);

export const handler = async (event) => {
  try {
    const data = event.data;
    
    await Promise.all(data.map(async (garden) => {
      const command = new PutCommand({
        TableName: 'IoTMonitoringGardenMetrics',
        Item: {
          GardenId: garden.id,
          Timestamp: new Date(garden.timestamp * 1000).toISOString(),
          Temperature: garden.temperature,
          Humidity: garden.humidity,
        }
      });
      
      await docClient.send(command);
    }));
    
    return {
      statusCode: 200,
      body: JSON.stringify({ message: 'Lambda function executed successfully' })
    };
  } catch (error) {
    console.error(`Error processing event: ${JSON.stringify(event)}`, error);
    return {
      statusCode: 500,
      body: JSON.stringify({ error: 'Internal Server Error' })
    };
  }
};
