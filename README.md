# 此文件为ucos源代码os_mbox.c函数解读文档

## function 1:	接收消息邮箱的消息函数

​		*ACCEPT MESSAGE FROM MAILBOX*

```c
void *OSMboxAccept(OS_EVENT *pevent)
```

```comments from C file
* Description: This function checks the mailbox to see if a message is available.  Unlike OSMboxPend(), OSMboxAccept() does not suspend the calling task if a message is not available.
* 该函数检查邮箱是否有消息可用，不像OSMboxPend()，当消息不可用时该函数不可以阻塞函数调用栈。
* Arguments  : pevent        is a pointer to the event control block
* 参数：pevent 指向事件控制块的指针
* Returns    : != (void *)0  is the message in the mailbox if one is available.  The mailbox is cleared
*                            so the next time OSMboxAccept() is called, the mailbox will be empty.
* 			返回 != (void *)0，表示如果当前邮箱里的消息可用，返回的就是该消息。 该邮箱被清空，所以当下一个OSMboxAccept()被调用时，邮箱就会变空。
*              == (void *)0  if the mailbox is empty or,
*                            if 'pevent' is a NULL pointer or,
*                            if you didn't pass the proper event pointer.
* 			返回 == (void *)0，表示当前邮箱空、pevent为空指针或没有传递正确的事件指针。
```



## function 2:	创建消息邮箱

​		*CREATE A MESSAGE MAILBOX*

```c
OS_EVENT *OSMboxCreate(void *pmsg)
```

```comments from C file
* Description: This function creates a message mailbox if free event control blocks are available.
* 当自由事件控制块可用时，该函数创建一个消息邮箱。
* Arguments  : pmsg          is a pointer to a message that you wish to deposit in the mailbox.  If
*                            you set this value to the NULL pointer (i.e. (void *)0) then the mailbox
*                            will be considered empty.
* 							表示欲存入邮箱的消息指针。如果将该值设置为空指针（如 (void *)0）则该邮箱将被解析为空。
* Returns    : != (OS_EVENT *)0  is a pointer to the event control clock (OS_EVENT) associated with the
*                                created mailbox
			返回 != (OS_EVENT *)0，表示是一个与创建了的邮箱相关联的事件控制块(OS_EVENT)的指针。
*              == (OS_EVENT *)0  if no event control blocks were available
			返回 == (OS_EVENT *)0，表示没有事件控制块可用。
```



## function 3:	删除邮箱

​		*DELETE A MAILBOX*

```c
OS_EVENT *OSMboxDel(OS_EVENT *pevent, INT8U opt, INT8U *perr)
```

```comments from C file
* Description: This function deletes a mailbox and readies all tasks pending on the mailbox.
* 删除邮箱，然后将所有在邮箱内等待的任务全部进入就绪态。
* Arguments  : pevent        is a pointer to the event control block associated with the desired
*                            mailbox. 
							是指向与目标邮箱关联的事件控制块的指针。
* 
*              opt           determines delete options as follows: 确定删除的选项，如下所示。
*                            opt == OS_DEL_NO_PEND   Delete the mailbox ONLY if no task pending
* 												当且仅当没有任务等待时，删除邮箱。
*                            opt == OS_DEL_ALWAYS    Deletes the mailbox even if tasks are waiting.
*                                                    In this case, all the tasks pending will be readied.
*												有任务等待时，也删除邮箱。这样的话，所有等待任务都将进入就绪态。
* 
*              perr          is a pointer to an error code that can contain one of the following values: 错误代码指针。
*                            OS_ERR_NONE             The call was successful and the mailbox was deleted 表示调用成功，邮箱被删除。
*                            OS_ERR_DEL_ISR          If you attempted to delete the mailbox from an ISR	表示尝试从ISR删除邮箱。
*                            OS_ERR_INVALID_OPT      An invalid option was specified 表示无效opt选项。
*                            OS_ERR_TASK_WAITING     One or more tasks were waiting on the mailbox 表示一个或多个任务在邮箱等待。
*                            OS_ERR_EVENT_TYPE       If you didn't pass a pointer to a mailbox 表示未向邮箱传递指针。
*                            OS_ERR_PEVENT_NULL      If 'pevent' is a NULL pointer. 表示 pevent 为空指针。
*
* Returns    : pevent        upon error 上述错误。
*              (OS_EVENT *)0 if the mailbox was successfully deleted. 
			返回 (OS_EVENT *)0，表示调用OS_ERR_NONE成功，邮箱被删除。
*
* Note(s)    : 1) This function must be used with care.  Tasks that would normally expect the presence of
*                 the mailbox MUST check the return code of OSMboxPend().
				该函数必须要小心使用，那些通常依赖邮箱存在性的任务必须要检查OSMboxPend()的返回代码。
*              2) OSMboxAccept() callers will not know that the intended mailbox has been deleted!
				OSMboxAccept()的调用者不会知道预期的邮箱已经被删除。
*              3) This call can potentially disable interrupts for a long time.  The interrupt disable
*                 time is directly proportional to the number of tasks waiting on the mailbox.
				该调用有可能将长时间的禁用中断。中断禁用时间与等待任务的数量成正比。
*              4) Because ALL tasks pending on the mailbox will be readied, you MUST be careful in
*                 applications where the mailbox is used for mutual exclusion because the resource(s)
*                 will no longer be guarded by the mailbox.
				因为在邮箱内所有的等待任务将会进入就绪态，所以你在使用邮箱互斥的应用程序中必须小心！因为邮箱不再会保护资源。

```



## function 4:	在邮箱上等待消息送入

​		*PEND ON MAILBOX FOR A MESSAGE*

```c
void *OSMboxPend(OS_EVENT *pevent, INT16U timeout, INT8U *perr)
```

```comments from C file
* Description: This function waits for a message to be sent to a mailbox
*				该函数等待一个消息送入邮箱。
* Arguments  : pevent        is a pointer to the event control block associated with the desired mailbox
*							是指向与目标邮箱关联的事件控制块的指针。

*              timeout       is an optional timeout period (in clock ticks).  If non-zero, your task will
*                            wait for a message to arrive at the mailbox up to the amount of time
*                            specified by this argument.  If you specify 0, however, your task will wait
*                            forever at the specified mailbox or, until a message arrives.
*							是可选的时间周期（以时钟ticks为单位）。 如果不为零，任务将等待消息到参数指定的时间为止之后才到达邮箱。 但是，如果指定0，							则任务将永远在指定的邮箱中等待，或者直到消息到达为止。

*              perr          is a pointer to where an error message will be deposited.  Possible error
*                            messages are: 表示指向错误消息存放位置的指针。
*
*                            OS_ERR_NONE         The call was successful and your task received a
*                                                message. 表示该次调用成功，任务收到消息。
*                            OS_ERR_TIMEOUT      A message was not received within the specified 'timeout'. 表示在timeout时间周期内未收到消息。
*                            OS_ERR_PEND_ABORT   The wait on the mailbox was aborted. 表示邮箱等待被中止。
*                            OS_ERR_EVENT_TYPE   Invalid event type 表示无效事件类型。
*                            OS_ERR_PEND_ISR     If you called this function from an ISR and the result
*                                                would lead to a suspension. 表示是从ISR调用该函数，从而导致的挂起。
*                            OS_ERR_PEVENT_NULL  If 'pevent' is a NULL pointer 表示 pevent 为空指针。
*                            OS_ERR_PEND_LOCKED  If you called this function when the scheduler is locked 表示当调度程序锁上时调用了该函数。
*
* Returns    : != (void *)0  is a pointer to the message received 
			返回 != (void *)0, 表示收到了指向的消息的指针。
*              == (void *)0  if no message was received or,
*                            if 'pevent' is a NULL pointer or,
*                            if you didn't pass the proper pointer to the event control block.
			返回 == (void *)0，表示当前邮箱空、pevent为空指针或没有传递正确的事件指针。
```



## function 5:	终止邮箱等待

​		 *ABORT WAITING ON A MESSAGE MAILBOX*

```c
INT8U OSMboxPendAbort(OS_EVENT *pevent, INT8U opt, INT8U *perr)
```

```comments from C file
* Description: This function aborts & readies any tasks currently waiting on a mailbox.  This function 
*              should be used to fault-abort the wait on the mailbox, rather than to normally signal
*              the mailbox via OSMboxPost() or OSMboxPostOpt().
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired mailbox.
*							是指向与目标邮箱关联的事件控制块的指针。

*              opt           determines the type of ABORT performed: 确定进行何种中止操作。
*                            OS_PEND_OPT_NONE         ABORT wait for a single task (HPT) waiting on the
*                                                     mailbox
												中止等待单个任务（HPT）在邮箱上等待
*                            OS_PEND_OPT_BROADCAST    ABORT wait for ALL tasks that are  waiting on the
*                                                     mailbox
*												中止等待邮箱中所有正在等待的任务
*              perr          is a pointer to where an error message will be deposited.  Possible error
*                            messages are: 指向错误消息存放位置的指针。
*
*                            OS_ERR_NONE         No tasks were waiting on the mailbox. 表示没有任务在邮箱等待
*                            OS_ERR_PEND_ABORT   At least one task waiting on the mailbox was readied
*                                                and informed of the aborted wait; check return value 
*                                                for the number of tasks whose wait on the mailbox 
*                                                was aborted. 表示至少有一个任务在邮箱就绪，并通知任务该等待被中止。检查在邮箱等待的任务数量返回值。
*                            OS_ERR_EVENT_TYPE   If you didn't pass a pointer to a mailbox. 表示没有向邮箱传递指针。
*                            OS_ERR_PEVENT_NULL  If 'pevent' is a NULL pointer. 表示 pevent 为空指针。
*
* Returns    : == 0          if no tasks were waiting on the mailbox, or upon error.  
			返回 == 0 表示邮箱上没有等待任务，或者上述错误。
*              >  0          if one or more tasks waiting on the mailbox are now readied and informed. 
			返回 > 0 表示在邮箱上等待的一个或多个任务现在就绪并通知了。
* Note(s)    : 1) HPT means Highest Priority Task 高优先级任务
```



## function 6:	向邮箱发送消息（不带指定 POST 类型）

​		*POST MESSAGE TO A MAILBOX (DIDN'T SPECIFY POST TYPE)*

```c
INT8U OSMboxPost(OS_EVENT *pevent, void *pmsg)
```

```comments from C file
* Description: This function sends a message to a mailbox
* 				该函数向邮箱发送消息
* Arguments  : pevent        is a pointer to the event control block associated with the desired mailbox
							是指向与目标邮箱关联的事件控制块的指针。
*
*              pmsg          is a pointer to the message to send.  You MUST NOT send a NULL pointer.
							要发送的消息的指针。绝对不可以发送一个空指针！
*
* Returns    : OS_ERR_NONE          The call was successful and the message was sent 表示调用成功，消息已发送。
*              OS_ERR_MBOX_FULL     If the mailbox already contains a message.  You can can only send one
*                                   message at a time and thus, the message MUST be consumed before you
*                                   are allowed to send another one.
								表示如果当前邮箱已经包含有消息，只能一次发送一条消息，因此，在允许发送另一条时，（上一条必须要先处理好？）
*              OS_ERR_EVENT_TYPE    If you are attempting to post to a non mailbox. 表示向非邮箱发送消息。
*              OS_ERR_PEVENT_NULL   If 'pevent' is a NULL pointer 表示pevent为空指针。
*              OS_ERR_POST_NULL_PTR If you are attempting to post a NULL pointer 表示尝试发送了一个空指针。
*

```



## function 7:	向邮箱发送消息（指定 POST 类型）

​		*POST MESSAGE TO A MAILBOX (SPECIFIED POST TYPE)*

```c
INT8U OSMboxPostOpt(OS_EVENT *pevent, void *pmsg, INT8U opt)
```

```comments from C file
* Description: This function sends a message to a mailbox 同上
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired mailbox
*
*              pmsg          is a pointer to the message to send.  You MUST NOT send a NULL pointer.
*
*              opt           determines the type of POST performed: 确定进行何种POST操作。
*                            OS_POST_OPT_NONE         POST to a single waiting task
*                                                     (Identical to OSMboxPost())
												 向单个等待任务发POST，与OSMboxPost()等价。
*                            OS_POST_OPT_BROADCAST    POST to ALL tasks that are waiting on the mailbox
*												 向所有等待任务发POST。
*                            OS_POST_OPT_NO_SCHED     Indicates that the scheduler will NOT be invoked
*												 表示不会调用调度程序。
* Returns    : OS_ERR_NONE          The call was successful and the message was sent
*              OS_ERR_MBOX_FULL     If the mailbox already contains a message.  You can can only send one
*                                   message at a time and thus, the message MUST be consumed before you
*                                   are allowed to send another one.
*              OS_ERR_EVENT_TYPE    If you are attempting to post to a non mailbox.
*              OS_ERR_PEVENT_NULL   If 'pevent' is a NULL pointer
*              OS_ERR_POST_NULL_PTR If you are attempting to post a NULL pointer
*
* Note(s)    : 1) HPT means Highest Priority Task
*
* Warning    : Interrupts can be disabled for a long time if you do a 'broadcast'.  In fact, the
*              interrupt disable time is proportional to the number of tasks waiting on the mailbox.
			警告：如果进行“广播”操作，中断可能会被长时间中断。事实上，中断时间和邮箱等待任务的数量成正比。

```



## function 8:	检查邮箱状态信息

​		*QUERY A MESSAGE MAILBOX*

```c
INT8U OSMboxQuery(OS_EVENT *pevent, OS_MBOX_DATA *p_mbox_data)
```

```comments from C file
* Description: This function obtains information about a message mailbox.
* 				该函数包含一个消息邮箱的信息。
* Arguments  : pevent        is a pointer to the event control block associated with the desired mailbox
*							是指向与目标邮箱关联的事件控制块的指针。
*              p_mbox_data   is a pointer to a structure that will contain information about the message
*                            mailbox.
*							是指向将包含消息邮箱的信息的结构体。
* Returns    : OS_ERR_NONE         The call was successful and the message was sent 表示调用成功，消息已发送。
*              OS_ERR_EVENT_TYPE   If you are attempting to obtain data from a non mailbox. 表示向非邮箱发送消息。
*              OS_ERR_PEVENT_NULL  If 'pevent'      is a NULL pointer 表示 pevent 为空指针。
*              OS_ERR_PDATA_NULL   If 'p_mbox_data' is a NULL pointer 表示 p_mbox_data 为空指针。

```

