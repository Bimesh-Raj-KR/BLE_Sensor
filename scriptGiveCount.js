document.addEventListener('DOMContentLoaded', function()
{
    const postInput = document.getElementById('post-count');
    const putInput = document.getElementById('put-count');

    postInput.addEventListener('keypress', async function(event)
    {
        if ('Enter' === event.key)
        {
            const count = parseInt(this.value);

            if (true === isNaN(count))
            {
                return;
            }

            try
            {
                const response = await fetch('/Count/', 
                {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/json' },
                    body: JSON.stringify({count: count})
                });

                if (response.ok)
                {
                    this.value = '';
                }
            }
            catch (error)
            {
                console.error('Posting count has failed', error);
            }
        }
    });

    putInput.addEventListener('keypress', async function(event) 
    {
        if ('Enter' === event.key)
        {
            const count = parseInt(this.value);

            if (true === isNaN(count))
            {
                return;
            }

            try
            {
                const response = await fetch('/Count/', 
                {
                    method: 'PUT',
                    headers: { 'Content-Type': 'application/json' },
                    body: JSON.stringify({count: count})
                });

                if (response.ok)
                {
                    this.value = '';
                }
            }
            catch (error)
            {
                console.error('Putting count has failed', error);
            }
        }
    });
});
