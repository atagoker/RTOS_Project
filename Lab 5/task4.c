#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <alchemy/task.h>
#include <alchemy/buffer.h>
#include <alchemy/mutex.h>

RT_TASK writer_task;
RT_TASK reader_task1;
RT_TASK reader_task2;

RT_BUFFER buffer;

RT_MUTEX print_mutex;

void writer(void *arg)
{
    char message[] =
    "Fenerbahce Spor Kulubu, 1907 yilinda Istanbul’da kurulmus olan "
    "Turkiye’nin en buyuk ve en basarili spor kuluplerinden biridir. "
    "Kulup futbol, basketbol, voleybol ve diger bircok spor dalinda "
    "faaliyet gostermektedir. Fenerbahce’nin ic saha maclarini oynadigi "
    "Ulker Stadyumu Sukru Saracoglu Spor Kompleksi, Istanbul’un Kadikoy "
    "ilcesinde yer almaktadir. Kulubun renkleri sari ve laciverttir. "
    "Fenerbahce taraftari Turkiye’nin en buyuk taraftar topluluklarindan "
    "birisi olarak kabul edilmektedir. Takim tarihinde bircok lig "
    "sampiyonlugu ve ulusal kupa bulunmaktadir. Avrupa kupalarinda da "
    "onemli basarilar elde eden kulup, ozellikle UEFA Avrupa Ligi’nde "
    "yari final oynayarak buyuk dikkat cekmistir. Fenerbahce sadece bir "
    "futbol takimi degil, ayni zamanda buyuk bir spor kulturunun ve "
    "toplulugunun temsilcisidir. Kulubun rekabet halinde oldugu en buyuk "
    "rakiplerinden biri Galatasaray’dir ve bu maclar ‘Kitalararasi Derbi’ "
    "olarak bilinmektedir.";

    int i;

    for (i = 1; i <= 5; i++)
    {
        rt_buffer_write(&buffer,
                        message,
                        strlen(message) + 1,
                        TM_INFINITE);

        rt_mutex_acquire(&print_mutex, TM_INFINITE);

        printf("\nWriter sent large text chunk %d\n", i);

        rt_mutex_release(&print_mutex);

        sleep(1);
    }
}

void reader(void *arg)
{
    char received[4096];

    int i;

    for (i = 1; i <= 5; i++)
    {
        rt_buffer_read(&buffer,
                       received,
                       sizeof(received),
                       TM_INFINITE);

        rt_mutex_acquire(&print_mutex, TM_INFINITE);

        printf("\nReader received:\n%s\n", received);

        rt_mutex_release(&print_mutex);
    }
}

int main()
{
    rt_buffer_create(&buffer,
                     "TextBuffer",
                     16384,
                     B_FIFO);

    rt_mutex_create(&print_mutex,
                    "PrintMutex");

    rt_task_create(&writer_task,
                   "WriterTask",
                   0,
                   50,
                   0);

    rt_task_create(&reader_task1,
                   "ReaderTask1",
                   0,
                   50,
                   0);

    rt_task_create(&reader_task2,
                   "ReaderTask2",
                   0,
                   50,
                   0);

    rt_task_start(&writer_task,
                  &writer,
                  NULL);

    rt_task_start(&reader_task1,
                  &reader,
                  NULL);

    rt_task_start(&reader_task2,
                  &reader,
                  NULL);

    sleep(7);

    rt_buffer_delete(&buffer);

    rt_mutex_delete(&print_mutex);

    return 0;
}
