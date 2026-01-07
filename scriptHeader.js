async function fetchHeader() 
{
    try {
        const response = await fetch('/getHeaderFile/header.json');

        if (!response.ok) {
            throw new Error('Network response was not ok');
        }

        const binaryData = await response.blob(); 
        const filContent = await binaryData.text();
        const data = JSON.parse(filContent);

        document.getElementById('page-title').textContent = data.Title;
        document.getElementById('add-title').textContent = data.Brackets;
    } 

    catch (error) {
        console.error('There has been a problem with fetch operation:', error);
    }
}

setTimeout(fetchHeader, 500);
