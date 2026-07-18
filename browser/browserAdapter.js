// Browser Adapter
//
// This file handles Playwright and Chromium.
// C++ side only talks to this adapter using HTTP/JSON.
// If Playwright is replaced, only this file needs changes.
//
// Flow:
// C++ PageFetcher → HTTP request → Browser Adapter → Playwright → HTML response

const express = require('express');
const { chromium } = require('playwright');

const PORT = process.env.BROWSER_ADAPTER_PORT || 3000;

const NAVIGATION_TIMEOUT_MS = process.env.NAVIGATION_TIMEOUT_MS
  ? parseInt(process.env.NAVIGATION_TIMEOUT_MS, 10)
  : 30000;

const app = express();
app.use(express.json());


// Stores the browser instance.
// Browser starts once and is reused for all requests.
let browser = null;



// Component: Health Check
//
// Used by PageFetcher to check if browser is ready.
app.get('/health', (req, res) => {
  res.json({ status: browser ? 'ok' : 'browser_not_ready' });
});



// Component: Page Fetcher Endpoint
//
// Receives URL from C++ side.
// Opens a new page, loads the website,
// gets rendered HTML and sends it back.
app.post('/fetch', async (req, res) => {
  const { url } = req.body || {};

  // Validate request data.
  if (!url || typeof url !== 'string') {
    return res.json({
      success: false,
      error: 'Request body must contain a non-empty "url" string.'
    });
  }


  // Browser should be running before fetching.
  if (!browser) {
    return res.json({
      success: false,
      error: 'Browser is not initialized.'
    });
  }


  let page = null;

  try {

    // Create a new page for this request.
    page = await browser.newPage();


    // Route requests to block images, stylesheets, fonts, and media files.
    // This saves bandwidth, CPU, and dramatically speeds up page load times.
    await page.route('**/*', (route) => {
      const resourceType = route.request().resourceType();
      if (['image', 'stylesheet', 'font', 'media'].includes(resourceType)) {
        route.abort();
      } else {
        route.continue();
      }
    });


    // Load page and wait until the load event fires (much faster than networkidle).
    await page.goto(url, {
      waitUntil: 'load',
      timeout: NAVIGATION_TIMEOUT_MS,
    });


    // Get final rendered HTML.
    const html = await page.content();


    return res.json({
      success: true,
      html
    });


  } catch (err) {

    // Send error back to PageFetcher.
    return res.json({
      success: false,
      error: err.message || String(err)
    });


  } finally {

    // Close only this page.
    // Browser stays alive for next request.
    if (page) {
      try {
        await page.close();
      } catch (_) {
        // Ignore close errors.
      }
    }
  }
});



// Component: Browser Startup
//
// Starts Chromium once and starts HTTP server.
async function main() {

  browser = await chromium.launch({
    headless: true
  });

  console.log('Browser Adapter: Chromium launched.');


  const server = app.listen(PORT, () => {
    console.log(`Browser Adapter: listening on http://localhost:${PORT}`);
  });



  // Component: Graceful Shutdown
  //
  // Closes server and browser safely.
  const shutdown = async () => {

    console.log('Browser Adapter: shutting down...');


    server.close();


    if (browser) {
      await browser.close();
    }


    process.exit(0);
  };


  process.on('SIGINT', shutdown);
  process.on('SIGTERM', shutdown);
}



// Start adapter.
main().catch((err) => {

  console.error('Browser Adapter: fatal startup error:', err);

  process.exit(1);

});