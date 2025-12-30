async function fetchTime() 
{
    try {
        const response = await fetch('/time/');
        if (!response.ok) {
            throw new Error('Network response was not ok');
        }
        const data = await response.json();
        console.log(data);

        document.getElementById('date-display').textContent = data.Date;
        document.getElementById('time-display').textContent = data.Time;
    } 

    catch (error) {
        console.error('There has been a problem with fetch operation:', error);
    }
}

setInterval(fetchTime, 1000);
fetchTime();
