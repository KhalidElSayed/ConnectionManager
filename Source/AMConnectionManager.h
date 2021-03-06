//
//  AMConnectionManager.h
//  ConnectionManager
//
//  Created by Joan Martin on 10/3/12.
//  Copyright (c) 2012 Joan Martin. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "AMAsyncConnectionOperation.h"

extern NSString * const AMConnectionManagerConnectionsDidStartNotification;
extern NSString * const AMConnectionManagerConnectionsDidFinishNotification;
extern NSString * const AMConnectionManagerConnectionsQueueIdentifierKey;

extern NSString * const AMConnectionManagerDefaultQueueIdentifier;

/*!
 * @typedef AMConnectionPriority
 * @abstract These are the available priorities to assign to connection requests.
 * @constant AMConnectionPriorityVeryLow Lower priority.
 * @constant AMConnectionPriorityLow This priority is higher than AMConnectionPriorityVeryLow and lower than others.
 * @constant AMConnectionPriorityNormal This is the default priority for connections. Also it is placed in the middle of all priorities.
 * @constant AMConnectionPriorityHigh The second placed priority.
 * @constant AMConnectionPriorityVeryHigh The highest priority.
 * @discussion The AMConnectionPriority is equivalent to NSOperationQueuePriority.
 */
typedef enum
{
    AMConnectionPriorityVeryLow = NSOperationQueuePriorityVeryLow,
    AMConnectionPriorityLow = NSOperationQueuePriorityLow,
    AMConnectionPriorityNormal = NSOperationQueuePriorityNormal,
    AMConnectionPriorityHigh = NSOperationQueuePriorityHigh,
    AMConnectionPriorityVeryHigh = NSOperationQueuePriorityHigh
} AMConnectionPriority;

@class AMConcurrentOperation;
@class AMAsyncConnectionOperation;

/*!
 * This class manages connection requests asynchronously in order to control the concurrent executions. User can set the maximum concurrent number of connections, cancel queued requests, give priorities, etc. This class implements the singleton pattern in order to share a single instance.
 */
@interface AMConnectionManager : NSObject

/*!
 * Returns the default manager instance in order to use this class as a singleton.
 * @return The default manager instance.
 */
+ (AMConnectionManager*)defaultManager;

/*!
 * This property allows to turn on/off the UIApplication's networkActivityIndicator while requests are being performed.
 */
@property (nonatomic, assign) BOOL showsNetworkActivityIndicator;

/*!
 * Set the maximum number of concurrent connections of the default queue. By default (-1), the maximum number of operations is determined dynamically using the current system conditions.
 */
@property (nonatomic, readwrite) NSInteger maxConcurrentConnectionCount;

/*!
 * Add trusted hosts for specific requests that uses credentials.
 */
@property (nonatomic, strong) NSArray *trustedHosts;

/*!
 * When the attribute is set to YES (the default) the connection manager present alert views when connection errors occurs.
 */
@property (nonatomic, assign) BOOL showConnectionErrors;

/*!
 * Configure the max number of concurrent connections for a specific queue.
 * @param maxConcurrentConnectionCount The maximum number of connections. Specify -1 (default) and the system will determine the value automatically.
 */
- (void)setMaxConcurrentConnectionCount:(NSInteger)maxConcurrentConnectionCount inQueue:(NSString*)queueIdentifier;

/*!
 * This method allow request cancelation.
 * @param key The request key.
 * @discussion If the request has been already executed or the key is unknown, this method does nothing.
 * @return The method return a new copy of the connection operation that can be reused to perform the connection again if needed.
 */
- (AMAsyncConnectionOperation*)cancelRequestWithKey:(NSInteger)key;

/*!
 * Changes the priority of the request associated to the given key.
 * @param priority The new priority.
 * @param key The request identifier.
 * @discussion If the request has been already executed or the key is unknown, this method does nothing.
 */
- (void)changeToPriority:(AMConnectionPriority)priority requestWithKey:(NSInteger)key;

/*!
 * This method freezes the queue with the given identifier: supsends the queue and pauses the executing connections.
 * @param identifier The queue identifier. Pass nil to refere to the default queue.
 * @discussion Because it is not possible to pause a connection, this method cancel the executing connections and these can be fired again calling the -unfreezeQueueWithIdentifier: method.
 */
- (void)freezeQueueWithIdentifier:(NSString*)identifier;

/*!
 * This method unfreezes the queue with the given identifier: restarts the queue and the paused executing connections.
 * @param identifier The queue identifier. Pass nil to refere to the default queue.
 * @discussion Because it is not possible to pause a connection, this method fires from scratch the paused connections. The corresponding paused connections keys are keept the same.
 */
- (void)unfreezeQueueWithIdentifier:(NSString*)identifier;

/*!
 * Calling this method all the queues are stopped and all the current executing operations are paused.
 * @discussion This method calls -freezeQueueWithIdentifier: for all the given identifiers.
 */
- (void)freeze;

/*!
 * This method unpauses all the paused operations and restart queues again.
 * @discussion This method calls -unfreezeQueueWithIdentifier: for all the given identifiers.
 */
- (void)unfreeze;

/*!
 * Returns the operation queue for the given identifier.
 * @param identifier The queue identifier. Pass nil to get the default queue.
 * @return The operation queue.
 * @discussion You can use this method in order to configure a queue manually.
 */
- (NSOperationQueue*)operationQueueForIdentifier:(NSString*)identifier;

/*!
 * Use this method to add manually a connection operation (AMConnectionOperation or AMAsyncConnectionOperation). This method enqueue the operation to the specified queue.
 * @param operation The operation to execute.
 * @param queueIdentifier An identifier of the queue. Pass nil to use the default queue.
 * @return The method returns an integer used as a key to identify the request. This identifier can be used in order to cancel the request.
 */
- (NSInteger)performConnectionOperation:(AMAsyncConnectionOperation*)operation inQueue:(NSString*)queueIdentifier;

/*!
 * Use this method to perform a request connection asynchronously and get back the response in the main thread.
 * @param request The request to perform.
 * @param completion The completion block with the response, the data and the error (in case of any problems).
 * @return The method returns an integer used as a key to identify the request. This identifier can be used in order to cancel the request.
 * @discussion The priority of this request is setted to AMConnectionPriorityNormal and the result operation will be inserted into the default queue.
 */
- (NSInteger)performRequest:(NSURLRequest*)request
            completionBlock:(void (^)(NSURLResponse* response, NSData* data, NSError* error, NSInteger key))completion;

/*!
 * Use this method to perform a request connection asynchronously and get back the response in the main thread.
 * @param request The request to perform.
 * @param queueIdentifier The identifier of the queue to perform the request. USe nil to use the default queue.
 * @param completion The completion block with the response, the data and the error (in case of any problems).
 * @return The method returns an integer used as a key to identify the request. This identifier can be used in order to cancel the request.
 * @discussion The priority of this request is setted to AMConnectionPriorityNormal and the result operation will be inserted into the default queue.
 */
- (NSInteger)performRequest:(NSURLRequest*)request
                    inQueue:(NSString*)queueIdentifier
            completionBlock:(void (^)(NSURLResponse* response, NSData* data, NSError* error, NSInteger key))completion;

/*!
 * Use this method to perform a request connection asynchronously and get back the response in the main thread with a given priority.
 * @param request The request to perform.
 * @param priority The request priority.
 * @param completion The completion block with the response, the data and the error (in case of any problems).
 * @return The method returns an integer used as a key to identify the request. This identifier can be used in order to cancel the request.
 * @discussion The result operation will be inserted into the default queue.
 */
- (NSInteger)performRequest:(NSURLRequest*)request
                   priority:(AMConnectionPriority)priority
            completionBlock:(void (^)(NSURLResponse* response, NSData* data, NSError* error, NSInteger key))completion;

/*!
 * This methods performs asynchornously a connection request.
 * @param request The request to perform.
 * @param progressStatus This block is potentially called multiple times, containing in the dictionary information about the headers and download & upload progress.
 * @param completionBlock This block is called when the connection ends.
 * @return The method returns an integer used as a key to identify the request. This identifier can be used in order to cancel the request.
 * @discussion The priority of this request is setted to AMConnectionPriorityNormal and the result operation will be inserted into the default queue.
 */
- (NSInteger)performRequest:(NSURLRequest*)request
             progressStatus:(void (^)(NSDictionary *progressStatus))progressStatusBlock
            completionBlock:(void (^)(NSURLResponse* response, NSData* data, NSError* error, NSInteger key))completion;

/*!
 * This methods performs asynchornously a connection request.
 * @param request The request to perform.
 * @param priority The request priority.
 * @param progressStatus This block is potentially called multiple times, containing in the dictionary information about the headers and download & upload progress.
 * @param completionBlock This block is called when the connection ends.
 * @return The method returns an integer used as a key to identify the request. This identifier can be used in order to cancel the request.
 * @discussion The result operation will be inserted into the default queue.
 */
- (NSInteger)performRequest:(NSURLRequest*)request
                   priority:(AMConnectionPriority)priority
             progressStatus:(void (^)(NSDictionary *progressStatus))progressStatusBlock
            completionBlock:(void (^)(NSURLResponse* response, NSData* data, NSError* error, NSInteger key))completion;

/*!
 * This methods performs asynchornously a connection request.
 * @param request The request to perform.
 * @param priority The request priority.
 * @param queueIdentifier The identifier of the queue to perform the request. USe nil to use the default queue.
 * @param progressStatus This block is potentially called multiple times, containing in the dictionary information about the headers and download & upload progress.
 * @param completionBlock This block is called when the connection ends.
 * @return The method returns an integer used as a key to identify the request. This identifier can be used in order to cancel the request.
 * @discussion The result operation will be inserted into the default queue.
 */
- (NSInteger)performRequest:(NSURLRequest*)request
                   priority:(AMConnectionPriority)priority
                    inQueue:(NSString*)queueIdentifier
             progressStatus:(void (^)(NSDictionary *progressStatus))progressStatusBlock
            completionBlock:(void (^)(NSURLResponse* response, NSData* data, NSError* error, NSInteger key))completion;

@property (nonatomic, strong) NSSet *backgroundExecutionQueueIdentifiers;

- (void)addBackgroundExecutionQueueIdentifier:(NSString*)queueIdentifier;
- (void)removeBackgroundExecutionQueueIdentifier:(NSString*)queueIdentifier;

@end
