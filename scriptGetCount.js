async function fetchCount() 
{
    try {
        const response = await fetch('/Count/?file=count.json');
    
        if (!response.ok) {
            throw new Error('Network response was not ok');
        }

        const binaryData = await response.blob(); 
        const filContent = await binaryData.text();
        const data = JSON.parse(filContent);

        document.getElementById('get-count').textContent = data.count;
    } 

    catch (error) {
        console.error('There has been a problem with fetch operation:', error);
    }
}

setInterval(fetchCount, 1000);
