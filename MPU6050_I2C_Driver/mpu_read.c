
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
int main(){

    u_int8_t buffer[14],ret;
    int fd = open("/dev/mpu60500",O_RDONLY);
    if(fd < 0){
        perror("file open failed\n");
        return -1;
    }

    ret = read(fd,buffer,14);

    int16_t ax,ay,az,t,gx,gy,gz;
    float Ax,Ay,Az,Temp,Gx,Gy,Gz;

    ax = (int16_t)((buffer[0] << 8) | buffer[1]);

    ay = (int16_t)((buffer[2] << 8) | buffer[3]);
    az = (int16_t)((buffer[4] << 8) | buffer[5]);
    t = (int16_t)((buffer[6] << 8) | buffer[7]);
    gx = (int16_t)((buffer[8] << 8) | buffer[9]);

    gy = (int16_t)((buffer[10] << 8) | buffer[11]);
    gz = (int16_t)((buffer[12] << 8) | buffer[13]);

	Ax = (float)ax / 16384.0f;   // ±2g full scale
    Ay = (float)ay / 16384.0f;
    Az = (float)az / 16384.0f;

    Temp = (float)t / 340.0f + 36.53f;

    Gx = (float)gx / 131.0f;     // ±250 °/s
    Gy = (float)gy / 131.0f;
    Gz = (float)gz / 131.0f;

	char msg[50];
sprintf(msg,"Ax = %.2f Ay = %.2f Az = %.2f\n Temp = %.2f\n Gx = %.2f Gy = %.2f Gz = %.2f\n",Ax,Ay,Az,T\
emp,Gx,Gy,Gz);

	printf("%s\n",msg);

	close(fd);


    return 0;
}
