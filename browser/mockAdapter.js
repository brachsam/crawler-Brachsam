// Mock Browser Adapter for testing.
//
// This does not use Playwright.
// It only checks that C++ PageFetcher can communicate correctly
// using the same HTTP request/response format as the real adapter.

const express = require('express');

const app = express();

app.use(express.json());



// Component: Health Check
//
// Used to verify that the adapter is running.
app.get('/health', (req, res) => {
  res.json({ status: 'ok' });
});



// Component: Fetch Mock HTML
//
// Simulates the real /fetch endpoint.
// Returns success or error responses like the real Browser Adapter.
app.post('/fetch', (req, res) => {

  const { url } = req.body || {};


  // Validate URL input.
  if (!url || typeof url !== 'string') {
    return res.json({
      success: false,
      error: 'Request body must contain a non-empty "url" string.'
    });
  }



  // Simulate timeout error.
  if (url === 'https://trigger-timeout.test') {
    return res.json({
      success: false,
      error: 'Navigation timeout of 30000ms exceeded'
    });
  }



  // Simulate invalid URL error.
  if (url === 'https://trigger-invalid.test') {
    return res.json({
      success: false,
      error: 'net::ERR_NAME_NOT_RESOLVED at ' + url
    });
  }



  // Return fake rendered HTML.
  // Used to verify URL communication through the HTTP layer.
  res.json({
    success: true,
    html: `<html><body><h1>Mock render of ${url}</h1></body></html>`,
  });

});



// Start mock server.
const port = process.env.MOCK_PORT || 3000;

app.listen(port, () =>
  console.log(`Mock adapter listening on ${port}`)
);