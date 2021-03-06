
/**********************************************************************
* 统计字符数，不包括最后的'\0'
**********************************************************************/
int strlen(const char *str)
{
	int i = 0;
	while(str[i++]);
	return i;
}

/**********************************************************************
* 返回值复制的字符数，包括最后的'\0'
**********************************************************************/
int strcpy(char *to,const char *from)
{
	int i = 0;
	while(from[i]){
		to[i]=from[i];
		i++;
	}
	to[i]='\0';
	return i+1;
}
